#include "AI_BEHAVIORAL.h"
#include "Menu_Log.hpp"

using namespace AI_BEHAVIORAL;

__declspec(noinline)
void __fastcall WithdrawalManager::h_request_withdraw(void* _this, void* _EDX, float initial_radius, float timed_out_radius, float time_to_force, bool permanent)
{
	// Steal the this pointer (this will definitely not work across level loads).
	m_this = _this;

	logger.AddLog("[WithdrawlManager] Got request to withdraw the Alien:\n\t\
		Initial radius: %.2f\n\t\
		Timed out radius: %.2f\n\t\
		Time to force: %.2f\n\t\
		Permanent: %s\n", initial_radius, timed_out_radius, time_to_force, (permanent ? "Yes" : "No"));

	request_withdraw(_this, initial_radius, timed_out_radius, time_to_force, permanent);
}

__declspec(noinline)
void __fastcall WithdrawalManager::h_set_withdraw_state(void* _this, void* _EDX, WITHDRAW_STATE state)
{
	logger.AddLog("[WithdrawlManager] Got request to set Alien withdrawl state:\n\t\
		New Withdraw State: %d\n", state);

	set_withdraw_state(_this, WITHDRAW_STATE::NeedsToWithdraw);
}
