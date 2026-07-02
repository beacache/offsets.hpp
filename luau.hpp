#pragma once
#include "roblox.h"

#pragma warning(disable: 4003)
#pragma warning(disable: 4172)

// dumped by sc(@k1ci0)
// roblox version: version-5cf2272675e145f5

// dumped in 147432ms (147.432s)
/*
              SET                                 GET
    vmvalue0: data = value                     || value = data
    vmvalue1: data = (value + (data + offset)) || value = (data - (value + offset))
    vmvalue2: data = ((data + offset) - value) || value = ((v + offset) - data)
    vmvalue3: data = (value ^ (data + offset)) || value = ((v + offset) ^ data)
    vmvalue4: data = (value - (data + offset)) || value = ((v + offset) + data)
*/

#define proto_lineinfo     vmvalue0/* proto_lineinfo_enc */
#define proto_debuginsn    vmvalue2/* proto_debuginsn_enc */
#define proto_typeinfo     vmvalue1/* proto_typeinfo_enc */
#define proto_abslineinfo  vmvalue3/* proto_abslineinfo_enc */
#define proto_source       vmvalue1/* proto_source_enc */
#define proto_locvars      vmvalue0/* proto_locvars_enc */
#define proto_upvalues     vmvalue0/* proto_upvalues_enc */
#define proto_debugname    vmvalue0/* proto_debugname_enc */
#define proto_userdata     vmvalue0/* proto_userdata_enc */
#define udata_meta         vmvalue0/* udata_meta_enc */
#define closure_debugname  vmvalue4/* closure_debugname_enc */
#define closure_cont       vmvalue4/* closure_cont_enc */
#define lstate_stacksize   vmvalue3/* lstate_stacksize_enc */
