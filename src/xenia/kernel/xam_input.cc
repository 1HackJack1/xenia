/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <xenia/kernel/xam_input.h>

#include <xenia/emulator.h>
#include <xenia/hid/hid.h>
#include <xenia/kernel/kernel_state.h>
#include <xenia/kernel/xam_private.h>
#include <xenia/kernel/util/shim_utils.h>


using namespace xe;
using namespace xe::hid;
using namespace xe::kernel;
using namespace xe::kernel::xam;


namespace xe {
namespace kernel {


// http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetcapabilities(v=vs.85).aspx
SHIM_CALL XamInputGetCapabilities_shim(
    PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t flags = SHIM_GET_ARG_32(1);
  uint32_t caps_ptr = SHIM_GET_ARG_32(2);

  XELOGD(
      "XamInputGetCapabilities(%d, %.8X, %.8X)",
      user_index,
      flags,
      caps_ptr);

  if (!caps_ptr) {
    SHIM_SET_RETURN(X_ERROR_BAD_ARGUMENTS);
    return;
  }

  InputSystem* input_system = state->emulator()->input_system();

  X_INPUT_CAPABILITIES caps;
  X_RESULT result = input_system->GetCapabilities(user_index, flags, caps);
  if (XSUCCEEDED(result)) {
    caps.Write(SHIM_MEM_BASE, caps_ptr);
  }
  SHIM_SET_RETURN(result);
}


// http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx
SHIM_CALL XamInputGetState_shim(
    PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t state_ptr = SHIM_GET_ARG_32(1);

  XELOGD(
      "XamInputGetState(%d, %.8X)",
      user_index,
      state_ptr);

  if (!state_ptr) {
    SHIM_SET_RETURN(X_ERROR_BAD_ARGUMENTS);
    return;
  }

  InputSystem* input_system = state->emulator()->input_system();

  X_INPUT_STATE input_state;
  X_RESULT result = input_system->GetState(user_index, input_state);
  if (XSUCCEEDED(result)) {
    input_state.Write(SHIM_MEM_BASE, state_ptr);
  }
  SHIM_SET_RETURN(result);
}


// http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputsetstate(v=vs.85).aspx
SHIM_CALL XamInputSetState_shim(
    PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t vibration_ptr = SHIM_GET_ARG_32(1);

  XELOGD(
      "XamInputSetState(%d, %.8X)",
      user_index,
      vibration_ptr);

  if (!vibration_ptr) {
    SHIM_SET_RETURN(X_ERROR_BAD_ARGUMENTS);
    return;
  }

  InputSystem* input_system = state->emulator()->input_system();

  X_INPUT_VIBRATION vibration(SHIM_MEM_BASE, vibration_ptr);
  X_RESULT result = input_system->SetState(user_index, vibration);
  SHIM_SET_RETURN(result);
}


// http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetkeystroke(v=vs.85).aspx
SHIM_CALL XamInputGetKeystroke_shim(
    PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t flags = SHIM_GET_ARG_32(1);
  uint32_t keystroke_ptr = SHIM_GET_ARG_32(2);

  // http://ffplay360.googlecode.com/svn/Test/Common/AtgXime.cpp
  // user index = index or XUSER_INDEX_ANY
  // flags = XINPUT_FLAG_GAMEPAD (| _ANYUSER | _ANYDEVICE)

  XELOGD(
      "XamInputGetKeystroke(%d, %.8X, %.8X)",
      user_index,
      flags,
      keystroke_ptr);

  if (!keystroke_ptr) {
    SHIM_SET_RETURN(X_ERROR_BAD_ARGUMENTS);
    return;
  }

  InputSystem* input_system = state->emulator()->input_system();

  X_INPUT_KEYSTROKE keystroke;
  X_RESULT result = input_system->GetKeystroke(user_index, flags, keystroke);
  if (XSUCCEEDED(result)) {
    keystroke.Write(SHIM_MEM_BASE, keystroke_ptr);
  }
  SHIM_SET_RETURN(result);
}


// Same as non-ex, just takes a pointer to user index.
SHIM_CALL XamInputGetKeystrokeEx_shim(
    PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index_ptr = SHIM_GET_ARG_32(0);
  uint32_t flags = SHIM_GET_ARG_32(1);
  uint32_t keystroke_ptr = SHIM_GET_ARG_32(2);

  uint32_t user_index = SHIM_MEM_32(user_index_ptr);

  XELOGD(
      "XamInputGetKeystroke(%.8X(%.d), %.8X, %.8X)",
      user_index_ptr, user_index,
      flags,
      keystroke_ptr);

  if (!keystroke_ptr) {
    SHIM_SET_RETURN(X_ERROR_BAD_ARGUMENTS);
    return;
  }

  InputSystem* input_system = state->emulator()->input_system();

  X_INPUT_KEYSTROKE keystroke;
  X_RESULT result = input_system->GetKeystroke(user_index, flags, keystroke);
  if (XSUCCEEDED(result)) {
    SHIM_SET_MEM_32(user_index_ptr, keystroke.user_index);
    keystroke.Write(SHIM_MEM_BASE, keystroke_ptr);
  }
  SHIM_SET_RETURN(result);
}


}  // namespace kernel
}  // namespace xe


void xe::kernel::xam::RegisterInputExports(
    ExportResolver* export_resolver, KernelState* state) {
  SHIM_SET_MAPPING("xam.xex", XamInputGetCapabilities, state);
  SHIM_SET_MAPPING("xam.xex", XamInputGetState, state);
  SHIM_SET_MAPPING("xam.xex", XamInputSetState, state);
  SHIM_SET_MAPPING("xam.xex", XamInputGetKeystroke, state);
  SHIM_SET_MAPPING("xam.xex", XamInputGetKeystrokeEx, state);
}
