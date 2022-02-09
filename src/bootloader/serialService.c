#include "include/serialService.h"

EFI_SERIAL_IO_PROTOCOL* serial_io_protocol;

EFI_STATUS configure_serial_protocol(IN EFI_SERIAL_IO_PROTOCOL* const protocol)
{
	/** The program status. */
	EFI_STATUS status;

	#ifdef MAPPLE_DEBUG
		Print(L"Debug: Configuring serial IO protocol\n");
	#endif

	status = uefi_call_wrapper(protocol->SetAttributes, 7,
		protocol, 0, 0, 0, 0, 0, DefaultStopBits);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error configuring Serial Protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	return EFI_SUCCESS;
}

EFI_STATUS init_serial_service(void)
{
	/** The program status. */
	EFI_STATUS status;

	#ifdef MAPPLE_DEBUG
		Print(L"Debug: Initialising Serial service\n");
	#endif

	status = uefi_call_wrapper(gBS->LocateProtocol, 3,
		&gEfiSerialIoProtocolGuid, NULL, &serial_io_protocol);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error locating Serial Protocol: %s\n",
			get_efi_error_message(status)
		);

		return status;
	}

	#ifdef MAPPLE_DEBUG
		Print(L"Debug: Located Serial Protocol\n");
	#endif

	status = configure_serial_protocol(serial_io_protocol);
	if(EFI_ERROR(status)) {
		return status;
	}

	return EFI_SUCCESS;
}
