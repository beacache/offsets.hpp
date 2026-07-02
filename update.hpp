#pragma once
#include <Windows.h>
#include <memory>
#include <atomic>
#include <string>
#include <cstdint>
#include <functional>

#include <encs.h>

// dumped by sc(@k1ci0)

// yes i steal this design from 'vylkyrie'
// roblox version: version-5cf2272675e145f5

template <typename T>
class offset_t {
    using custom_getter_t = std::function<T(std::uintptr_t)>;

    std::int32_t offset_value = NULL;
    custom_getter_t custom_getter;
public:
    offset_t(std::int32_t offset) : offset_value(offset) {};
    offset_t(std::int32_t offset, custom_getter_t getter) : offset_value(offset), custom_getter(getter) {};

    __forceinline std::int32_t get_offset() { return offset_value; }
    __forceinline std::uintptr_t bind(std::uintptr_t address) { return address + offset_value; }
    __forceinline T get(std::uintptr_t address) {
        if (custom_getter) return custom_getter(bind(address));
        return *reinterpret_cast<T*>(bind(address));
    }
    __forceinline void set(std::uintptr_t address, T value) { *reinterpret_cast<T*>(bind(address)) = value; }
    __forceinline void set_getter(custom_getter_t getter) { this->custom_getter = getter; }
};

struct lua_State;
namespace module::update {
    using luac_function_t = std::int32_t(*)(lua_State* lua_state);

    template <typename T>
    inline T rebase(std::uintptr_t rva) {
        return rva != NULL ? (T)(rva + reinterpret_cast<std::uintptr_t>(GetModuleHandleA(NULL))) : (T)(NULL);
    };

    namespace output {
        enum message_type_t : std::int32_t { basic, info, warn, error };
        using rbx_print_t = void(__fastcall*)(message_type_t message_type, const char* format, ...);

        const auto print = rebase<rbx_print_t>(0x4681920);
    };

    namespace luau {
        using luau_execute_t = void(__fastcall*)(lua_State* lua_state);
        using luad_throw_t = void(__fastcall*)(lua_State*, std::int32_t);
        using lua_vm_load_t = std::int32_t(__fastcall*)(lua_State*, std::string*, const char*, std::int32_t);
        using get_lua_state_for_instance_t = lua_State*(__fastcall*)(std::uintptr_t, std::uintptr_t*, std::uintptr_t*);
        using get_values_t = void(__fastcall*)(lua_State*, std::int32_t, void*, bool, std::int32_t);
        using disconnect_connection_t = void(__fastcall*)(std::uintptr_t*);

        const auto luao_nilobject = rebase<std::uintptr_t>(0x69833f0);
        const auto luah_dummynode = rebase<std::uintptr_t>(0x6983298);

        const auto luau_execute = rebase<luau_execute_t>(0x47128f0);
        const auto luad_throw = rebase<luad_throw_t>(0x47003e0);
        const auto task_defer = rebase<luac_function_t>(0x1e47630);

        const auto lua_vm_load = rebase<lua_vm_load_t>(0x1d1c840);
        const auto opcode_lookup_table = rebase<std::uintptr_t>(0x62176b0);
        const auto get_lua_state_for_instance = rebase<get_lua_state_for_instance_t>(0x1d03ff0);
        const auto get_values = rebase<get_values_t>(0x1d8b220);
        const auto disconnect_connection = rebase<disconnect_connection_t>(0x1d0b690);
    };

    namespace data_model {
        static auto place_id = offset_t<std::int64_t>(0x1a0); // was: 0x1a8
        static auto game_id = offset_t<std::int64_t>(0x1a8);
        static auto game_loaded = offset_t<std::uintptr_t>(0x658);
        static auto script_context = offset_t<std::uintptr_t>(0x440);
        static auto fake_dm_to_real_dm = offset_t<std::uintptr_t>(0x1d0);

        const auto fake_dm_pointer = rebase<std::uintptr_t>(0x7c3d2e8);
    };

    namespace script_context {
        struct debugger_result_t { std::int32_t result; std::int32_t unk[0x4]; };
        struct weak_thread_ref_t {
            std::atomic<std::int32_t> _refs;
            lua_State* thread;
            std::int32_t thread_ref;
            std::int32_t object_id;
            std::int32_t unk1;
            std::int32_t unk2;
        };

        using resume_t = std::uintptr_t(__thiscall*)(std::uintptr_t script_context, debugger_result_t* debugger_results, weak_thread_ref_t** weak_thread_ref, std::int32_t narg, bool resume_error, const std::string* error);

        const auto resume = rebase<resume_t>(0x1dd39e0);
        static auto resume_offset = offset_t<std::uintptr_t>(0x7e0);
    };

    namespace task_scheduler {
        const auto pointer = rebase<std::uintptr_t>(0x81cc868);

        static auto job_start = offset_t<std::uintptr_t>(0xc8);
        static auto job_end = offset_t<std::uintptr_t>(0xd0);
    };

    namespace extra_space {
        using get_identity_struct_t = std::uintptr_t(__fastcall*)(std::uintptr_t identity_pointer);
        using impersonator_t = std::uintptr_t(__fastcall*)(lua_State*, std::uintptr_t, std::uintptr_t*, std::int32_t);
        using get_capabilities_t = std::uintptr_t(__fastcall*)(std::uintptr_t);

        const auto get_capabilities = rebase<get_capabilities_t>(0x4760910);
        const auto get_identity_struct = rebase<get_identity_struct_t>(0xa120);
        const auto impersonator = rebase<impersonator_t>(0x1ce4550);
        const auto identity_pointer = rebase<std::uintptr_t>(0x81bd8e0);

        static auto require_bypass = offset_t<std::uintptr_t>(0x0); // not found
    };

    namespace scripts {
        static auto local_script_bytecode = offset_t<std::uintptr_t>(0x1a8);
        static auto module_script_bytecode = offset_t<std::uintptr_t>(0x150);

        static auto bytecode = offset_t<std::string>(0x10);
        static auto bytecode_size = offset_t<std::uint32_t>(0x20);
    };

    namespace script {
        namespace module_script {
            using get_from_vmstate_map_t = std::uintptr_t*(__fastcall*)(std::uintptr_t* vmstate_list, std::uintptr_t* vmstate, lua_State** main_thread);

            const auto get_from_vmstate_map = rebase<get_from_vmstate_map_t>(0x1d30f40);
        };
    };

    namespace instance_bridge {
        using push_instance_shared_ptr_t = std::uintptr_t(__cdecl*)(void* L, std::shared_ptr<std::uintptr_t*> instance);
        using push_instance_weak_ptr_t = std::uintptr_t(__cdecl*)(void* L, std::weak_ptr<std::uintptr_t*> instance);
        using push_instance_t = std::uintptr_t(__cdecl*)(void* L, void* instance);
        using push_instance_uint_ptr_t = void(__fastcall*)(lua_State*, std::uintptr_t);
        using push_instance_void_ptr_t = void(__fastcall*)(lua_State*, void**);
        using push_instance_void_ptr2_t = void(__fastcall*)(lua_State*, void*);
        using push_instance_weak_t = void(__fastcall*)(lua_State*, std::weak_ptr<std::uintptr_t>);

        const auto shared = rebase<push_instance_shared_ptr_t>(0x1cf6fb0);
        const auto weak = rebase<push_instance_weak_ptr_t>(0x1cf6fb0);
        const auto weak_uptr = rebase<push_instance_weak_t>(0x1cf6fb0);
        const auto uint_ptr = rebase<push_instance_uint_ptr_t>(0x1cf6fb0);
        const auto void_ptr = rebase<push_instance_void_ptr_t>(0x1cf6fb0);
        const auto void_ptr2 = rebase<push_instance_void_ptr2_t>(0x1cf6fb0);
    };

    namespace instance {
        static auto class_name = offset_t<std::string>(0x8);
        static auto scriptable_mask = offset_t<std::uint32_t>(0x10);
        static auto class_descriptor = offset_t<std::uintptr_t>(0x18);
        static auto prop_descriptor = offset_t<std::uintptr_t>(0x28);
        static auto parent = offset_t<std::uintptr_t>(0x70);
        static auto children = offset_t<std::uintptr_t>(0x78);
        static auto primitive = offset_t<std::uintptr_t>(0x148);
        static auto overlap = offset_t<std::uintptr_t>(0x1f0);
        static auto bit_flags = offset_t<std::uint32_t>(0x8c);
    };

    namespace class_descriptor {
        using get_property_t = std::uintptr_t*(__thiscall*)(std::uintptr_t, std::uintptr_t*);

        const auto ktable = rebase<std::uintptr_t*>(0x81ce200);
        const auto get_property = rebase<get_property_t>(0xc8a490);

        static auto gp_hashmap_offset = offset_t<std::uintptr_t>(0x250);
        static auto gp_bucket_idx_offset = offset_t<std::uintptr_t>(0x258);
        static auto gp_prop_table_offset = offset_t<std::uintptr_t>(0x260);
        static auto gp_bucket_mask_off = offset_t<std::uintptr_t>(0x268);
        static auto gp_shift_amount_off = offset_t<std::uintptr_t>(0x26c);
    };

    namespace raknet {
        using process_network_packet_t = void(__fastcall*)(void*, void*, char, std::uintptr_t, std::uint32_t, char);
        using report_network_error_t = void(__fastcall*)(std::uintptr_t, std::uint8_t*, char, std::uintptr_t, std::uint32_t, char);
        using handle_connection_state_t = void(__fastcall*)(std::uintptr_t, char*, std::int32_t, std::uint8_t*, char);

        const auto process_network_packet = rebase<process_network_packet_t>(0x31919d0);
        const auto report_network_error = rebase<report_network_error_t>(0xa49259);
        const auto handle_connection_state = rebase<handle_connection_state_t>(0xa54750);
    };

    namespace flags {
        const auto enable_load_module = rebase<std::uintptr_t>(0x7d54230);
        const auto lock_violation_instance_crash = rebase<std::uintptr_t>(0x7d4f050);
        const auto lock_violation_script_crash = rebase<std::uintptr_t>(0x7d51348);
        const auto wnd_process_check = rebase<std::uintptr_t>(0x77e3008);
        const auto lua_step_interval_ms_override_enabled = rebase<std::uintptr_t>(0x7d53318);
        const auto get_fast_flag = rebase<std::uintptr_t>(0x534e210);
        const auto set_fast_flag = rebase<std::uintptr_t>(0x0); // not found
        const auto task_scheduler_target_fps = rebase<std::uintptr_t>(0x49895e0);
    };

    namespace signals {
        using fireproximityprompt_t = std::uintptr_t*(__thiscall*)(std::uintptr_t);
        using fire_mouse_click_t = void(__fastcall*)(__int64, float, __int64);
        using fire_hover_t = void(__fastcall*)(__int64, __int64);
        using firetouchinterest_t = void(__fastcall*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, bool, bool);
        using cast_to_variant_t = std::uintptr_t(__fastcall*)(lua_State*, int, void*, bool, int);

        const auto cast_to_variant = rebase<cast_to_variant_t>(0x0); // not found
        const auto fire_mouse_click = rebase<fire_mouse_click_t>(0x0); // not found
        const auto fire_right_mouse_click = rebase<fire_mouse_click_t>(0x0); // not found
        const auto fire_mouse_hover_enter = rebase<fire_hover_t>(0x0); // not found
        const auto fire_mouse_hover_leave = rebase<fire_hover_t>(0x0); // not found
        const auto fire_proximity_prompt = rebase<fireproximityprompt_t>(0x266f870);
        const auto fire_touch_interest = rebase<firetouchinterest_t>(0x2a806c0);
    };

    namespace functions { // idk if this correct
        const auto task_spawn = rebase<luac_function_t>(0x1e48510);
        const auto task_delay = rebase<luac_function_t>(0x1e47a20);
        const auto task_wait = rebase<luac_function_t>(0x1e48890);
        const auto task_cancel = rebase<luac_function_t>(0x1e47320);
        const auto task_synchronize = rebase<luac_function_t>(0x1e48690);
        const auto task_desynchronize = rebase<luac_function_t>(0x1e47c70);

        const auto lua_pcall = rebase<std::uintptr_t>(0x46fdea0);

        const auto get_thread_data = rebase<std::uintptr_t>(0x6db6a0);

        const auto luaf_freeproto = rebase<std::uintptr_t>(0x472ae90);
        const auto luac_step = rebase<std::uintptr_t>(0x4704c60);
        const auto gcstep = rebase<std::uintptr_t>(0x4704740);
    };

    namespace appdata {
        using get_appdata_info_t = void*(__fastcall*)();

        const auto get = rebase<get_appdata_info_t>(0x36b0310);
        const auto info = rebase<std::uintptr_t>(0x7ea3ed0);
        static auto app_status = offset_t<std::int32_t>(0x28);
    };
}
