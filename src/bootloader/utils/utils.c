#include "include/utils.h"
#include "include/error.h"
/**
 * @brief Reset Timer
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS disableWatchDogTimer(void){
    // Disable the watchdog timer.
	CHECKER(
		uefi_call_wrapper(gBS->SetWatchdogTimer, 4, 0, 0, 0, NULL)
		,
		L"Fatal Error: Error setting watchdog timer: %s\n"
	);

    Print(L"Debug: WatchDog Timer Initalized\n");

    return EFI_SUCCESS;
};

/**
 * @brief Reset The Console
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS ResetConsole(EFI_SYSTEM_TABLE* SystemTable){
	// Reset console input.
	CHECKER(
		uefi_call_wrapper(ST->ConIn->Reset, 2, SystemTable->ConIn, FALSE)
		,
		L"Fatal Error: Error resetting console input: %s\n"
	);

	Print(L"Debug: Console Reset Done\n");

	return EFI_SUCCESS;
};