#include "include/serialService.h"

EFI_SERIAL_IO_PROTOCOL* serial_io_protocol;

EFI_STATUS configure_serial_protocol(IN EFI_SERIAL_IO_PROTOCOL* const protocol)
{

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Configuring serial IO protocol\n");
	#endif

	CHECKER(
		uefi_call_wrapper(protocol->SetAttributes, 7,
			protocol, 0, 0, 0, 0, 0, DefaultStopBits)
		,
		L"Error: Error configuring Serial Protocol: %s\n"
	);
	return EFI_SUCCESS;
}

EFI_STATUS init_serial_service(void)
{

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Initialising Serial service\n");
	#endif

	CHECKER(
		uefi_call_wrapper(gBS->LocateProtocol, 3,
			&gEfiSerialIoProtocolGuid, NULL, &serial_io_protocol)
		,
		L"Error: Error locating Serial Protocol: %s\n"
	);

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Located Serial Protocol\n");
	#endif

	CHECKER(
		configure_serial_protocol(serial_io_protocol)
		,
		L"Error: Unable to configure serial protocol, error: %s \n\r"
	);

	return EFI_SUCCESS;
}
