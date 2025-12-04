"""USBMux - Python wrapper for USB Mux controller."""

import serial
import serial.tools.list_ports

VID = 0x0483
PID = 0x5740


class USBMuxError(Exception):
    """Base exception for USBMux errors."""
    pass


class USBMuxFaultError(USBMuxError):
    """Raised when a VBUS fault is detected."""
    pass


class USBMuxNotFoundError(USBMuxError):
    """Raised when no USBMux device is found."""
    pass


class USBMux:
    """Controller for USB Mux device."""

    def __init__(self, port: str | None = None, timeout: float = 1.0):
        """
        Initialize USBMux connection.

        Args:
            port: Serial port path. Auto-detects if None.
            timeout: Serial timeout in seconds.

        Raises:
            USBMuxNotFoundError: If no device found during auto-detection.
        """
        if port is None:
            port = self._find_device()
        self._ser = serial.Serial(port, baudrate=115200, timeout=timeout)

    def _find_device(self) -> str:
        """Find USBMux device by VID/PID."""
        for p in serial.tools.list_ports.comports():
            if p.vid == VID and p.pid == PID:
                return p.device
        raise USBMuxNotFoundError(f"No USBMux device found (VID={VID:04X}, PID={PID:04X})")

    def _send(self, cmd: str) -> None:
        """Send command without expecting response."""
        self._ser.write(f"{cmd}\r\n".encode())

    def _query(self, cmd: str) -> str:
        """Send command and return response."""
        self._ser.reset_input_buffer()
        self._ser.write(f"{cmd}\r\n".encode())
        return self._ser.readline().decode().strip()

    def _check_fault(self, channel: int) -> None:
        """Check for fault on channel and raise if present."""
        f0, f1 = self.fault
        if channel == 0 and f0:
            raise USBMuxFaultError("Fault on channel 0 (overcurrent or thermal)")
        if channel == 1 and f1:
            raise USBMuxFaultError("Fault on channel 1 (overcurrent or thermal)")

    def close(self) -> None:
        """Close serial connection."""
        self._ser.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False

    @property
    def sel(self) -> int:
        """Get current mux selection (0 or 1)."""
        return int(self._query("SEL?"))

    @sel.setter
    def sel(self, value: int) -> None:
        """Set mux selection (0 or 1)."""
        self._send(f"SEL {value}")

    @property
    def oe(self) -> bool:
        """Get mux output enable state."""
        return self._query("OE?") == "1"

    @oe.setter
    def oe(self, value: bool) -> None:
        """Set mux output enable."""
        self._send(f"OE {1 if value else 0}")

    @property
    def en0(self) -> bool:
        """Get VBUS channel 0 enable state."""
        return self._query("EN0?") == "1"

    @en0.setter
    def en0(self, value: bool) -> None:
        """Set VBUS channel 0 enable."""
        self._send(f"EN0 {1 if value else 0}")
        if value:
            _ = self._query("EN0?")  # ensure command processed
            self._check_fault(0)

    @property
    def en1(self) -> bool:
        """Get VBUS channel 1 enable state."""
        return self._query("EN1?") == "1"

    @en1.setter
    def en1(self, value: bool) -> None:
        """Set VBUS channel 1 enable."""
        self._send(f"EN1 {1 if value else 0}")
        if value:
            _ = self._query("EN1?")  # ensure command processed
            self._check_fault(1)

    @property
    def fault(self) -> tuple[bool, bool]:
        """Get fault states (fault0, fault1)."""
        resp = self._query("FAULT?")
        parts = resp.split()
        return parts[0] == "1", parts[1] == "1"

    @property
    def status(self) -> dict:
        """Get full device status."""
        resp = self._query("STATUS?")
        result = {}
        for pair in resp.split():
            key, val = pair.split("=")
            result[key.lower()] = int(val)
        return result
