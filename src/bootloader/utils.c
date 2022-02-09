#include "include/utils.h"
#include "include/error.h"
/**
 * @brief Reset Timer
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS disableWatchDogTimer(void){
    EFI_STATUS status;
    // Disable the watchdog timer.
	status = uefi_call_wrapper(gBS->SetWatchdogTimer, 4, 0, 0, 0, NULL);
	if(EFI_ERROR(status)) {
		Print(L"Fatal Error: Error setting watchdog timer: %s\n",
			get_efi_error_message(status));
		return status;
	}

    Print(L"Debug: WatchDog Timer Initalized\n");

    return EFI_SUCCESS;
};

/**
 * @brief Reset The Console
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS ResetConsole(EFI_SYSTEM_TABLE* SystemTable){
	EFI_STATUS status;
	// Reset console input.
	status = uefi_call_wrapper(ST->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
	if(EFI_ERROR(status)) {
		Print(L"Fatal Error: Error resetting console input: %s\n",
			get_efi_error_message(status));

		return status;
	}

	Print(L"Debug: Console Reset Done\n");

	return EFI_SUCCESS;
};