#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace AI_BEHAVIORAL
{
	namespace WithdrawalManager
	{
		// Taken from the alien_in_vent behaviour tree.
		enum class WITHDRAW_STATE {
			NotWithdrawing,
			NeedsToWithdraw,
			Withdrawing
		};

		inline void* m_this = nullptr;

		// request_withdraw(WithdrawlManager* _this, float initial_radius, float timed_out_radius, float time_to_force, bool permanent)
		DEVTOOLS_DECLARE_CLASS_HOOK(void, request_withdraw, h_request_withdraw, t_request_withdraw, 0x002d0620, float initial_radius, float timed_out_radius, float time_to_force, bool permanent);

		// set_withdraw_state(WithdrawlManager* _this, WITHDRAW_STATE state)
		DEVTOOLS_DECLARE_CLASS_HOOK(void, set_withdraw_state, h_set_withdraw_state, t_set_withdraw_state, 0x0032fc20, WITHDRAW_STATE state);
	}
}
