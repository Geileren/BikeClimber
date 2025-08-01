using Plugin.BLE;
using Plugin.BLE.Abstractions;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using System.Diagnostics;
using System.Text;

namespace DataClimber.Services;

public class BleService
{
    private readonly IBluetoothLE _ble;
    private readonly IAdapter _adapter;
    private IDevice _device;
    private ICharacteristic _characteristic;

    public event Action<string> OnDataReceived;
    public event Action OnConnection;
    public event Action<string> OnError;

    public BleService()
    {
        _ble = CrossBluetoothLE.Current;
        _adapter = CrossBluetoothLE.Current.Adapter;
        _adapter.DeviceDiscovered += OnDeviceDiscovered;
    }

    public async Task StartScanAsync()
    {
        try
        {
            if (!_ble.IsOn)
            {
                Debug.WriteLine("Bluetooth er ikke tændt.");
                OnError?.Invoke("Bluetooth er ikke tændt");
                return;
            }

            Debug.WriteLine("Starter scanning...");

            // Stop any existing scan first
            if (_adapter.IsScanning)
            {
                await _adapter.StopScanningForDevicesAsync();
            }

            // Start scanning
            await _adapter.StartScanningForDevicesAsync();

            // Optional: Stop scanning after 10 seconds if no device found
            _ = Task.Delay(10000).ContinueWith(async _ =>
            {
                if (_adapter.IsScanning && _device == null)
                {
                    await _adapter.StopScanningForDevicesAsync();
                    Debug.WriteLine("Scanning timed out efter 10 sekunder");
                    OnError?.Invoke("Ingen ESP32 enheder fundet");
                }
            });
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Fejl ved scanning: {ex.Message}");
            OnError?.Invoke($"Scanning fejl: {ex.Message}");
        }
    }

    private async void OnDeviceDiscovered(object sender, DeviceEventArgs args)
    {
        try
        {
            Debug.WriteLine($"Fandt enhed: {args.Device.Name} - {args.Device.Id}");

            // Look for ESP32-C3-BLE (exact name from your ESP32 code)
            if (args.Device.Name?.Contains("ESP32-C3-BLE") == true)
            {
                Debug.WriteLine($"Fandt ESP32 enhed: {args.Device.Name}");
                _device = args.Device;

                await _adapter.StopScanningForDevicesAsync();

                // Connect to device
                Debug.WriteLine("Forsøger at forbinde...");
                await _adapter.ConnectToDeviceAsync(_device);
                Debug.WriteLine("Forbundet til device!");

                // Get services
                var services = await _device.GetServicesAsync();
                Debug.WriteLine($"Fandt {services.Count} services");

                // Look for our specific service UUID
                var serviceUuid = new Guid("12345678-1234-1234-1234-1234567890ab");
                var service = services.FirstOrDefault(s => s.Id == serviceUuid);

                if (service == null)
                {
                    Debug.WriteLine("Service ikke fundet!");
                    OnError?.Invoke("Service ikke fundet");
                    return;
                }

                Debug.WriteLine("Service fundet!");

                // Get characteristics
                var characteristics = await service.GetCharacteristicsAsync();
                Debug.WriteLine($"Fandt {characteristics.Count} karakteristikker");

                // Look for our specific characteristic UUID
                var characteristicUuid = new Guid("abcdefab-1234-5678-1234-abcdefabcdef");
                _characteristic = characteristics.FirstOrDefault(c => c.Id == characteristicUuid);

                if (_characteristic == null)
                {
                    Debug.WriteLine("Karakteristik ikke fundet!");
                    OnError?.Invoke("Karakteristik ikke fundet");
                    return;
                }

                Debug.WriteLine("Karakteristik fundet!");

                // Subscribe to notifications
                _characteristic.ValueUpdated += Characteristic_ValueUpdated;
                await _characteristic.StartUpdatesAsync();

                Debug.WriteLine("Forbundet og lytter efter data...");

                MainThread.BeginInvokeOnMainThread(() =>
                {
                    OnConnection?.Invoke();
                });
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Fejl ved device discovery: {ex.Message}");
            OnError?.Invoke($"Connection fejl: {ex.Message}");
        }
    }

    private void Characteristic_ValueUpdated(object sender, CharacteristicUpdatedEventArgs e)
    {
        try
        {
            var bytes = e.Characteristic.Value;
            if (bytes != null && bytes.Length > 0)
            {
                // Debug: Print raw bytes in both hex and decimal
                Debug.WriteLine($"[BLEService] Raw bytes ({bytes.Length}): {string.Join(", ", bytes.Select(b => $"{b:X2}({b})"))}");

                // Method 1: Simple UTF-8 decoding (original approach)
                var method1 = Encoding.UTF8.GetString(bytes);
                Debug.WriteLine($"[BLEService] Method 1 (direct UTF-8): '{method1}'");

                // Method 2: Remove null bytes only
                var method2Bytes = bytes.Where(b => b != 0).ToArray();
                var method2 = method2Bytes.Length > 0 ? Encoding.UTF8.GetString(method2Bytes) : "";
                Debug.WriteLine($"[BLEService] Method 2 (no nulls): '{method2}' (længde: {method2Bytes.Length})");

                // Method 3: ASCII decoding
                var method3 = Encoding.ASCII.GetString(bytes).TrimEnd('\0');
                Debug.WriteLine($"[BLEService] Method 3 (ASCII): '{method3}'");

                // Method 4: Manual byte-to-char conversion
                var method4 = string.Join("", bytes.Where(b => b >= 32 && b <= 126).Select(b => (char)b));
                Debug.WriteLine($"[BLEService] Method 4 (printable ASCII only): '{method4}'");

                // Method 5: Find null terminator and decode up to that point
                int nullIndex = Array.IndexOf(bytes, (byte)0);
                var method5Bytes = nullIndex >= 0 ? bytes.Take(nullIndex).ToArray() : bytes;
                var method5 = method5Bytes.Length > 0 ? Encoding.UTF8.GetString(method5Bytes) : "";
                Debug.WriteLine($"[BLEService] Method 5 (null terminated): '{method5}' (længde: {method5Bytes.Length})");

                // Choose the best result - prefer method 5 (null terminated), then method 2 (no nulls)
                string finalValue = "";
                if (!string.IsNullOrWhiteSpace(method5) && method5.All(c => c >= 32 && c <= 126))
                {
                    finalValue = method5;
                    Debug.WriteLine("[BLEService] Using method 5 (null terminated)");
                }
                else if (!string.IsNullOrWhiteSpace(method2) && method2.All(c => c >= 32 && c <= 126))
                {
                    finalValue = method2;
                    Debug.WriteLine("[BLEService] Using method 2 (no nulls)");
                }
                else if (!string.IsNullOrWhiteSpace(method3))
                {
                    finalValue = method3;
                    Debug.WriteLine("[BLEService] Using method 3 (ASCII)");
                }
                else if (!string.IsNullOrWhiteSpace(method4))
                {
                    finalValue = method4;
                    Debug.WriteLine("[BLEService] Using method 4 (printable only)");
                }
                else
                {
                    finalValue = method1; // Fallback to original
                    Debug.WriteLine("[BLEService] Using method 1 (direct UTF-8) as fallback");
                }

                Debug.WriteLine($"[BLEService] Final value: '{finalValue}'");

                MainThread.BeginInvokeOnMainThread(() =>
                {
                    Debug.WriteLine("[BLEService] Kalder OnDataReceived event");
                    OnDataReceived?.Invoke(finalValue);
                });
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Fejl ved ValueUpdated: {ex.Message}");
        }
    }

    public async Task DisconnectAsync()
    {
        try
        {
            if (_characteristic != null)
            {
                await _characteristic.StopUpdatesAsync();
                _characteristic.ValueUpdated -= Characteristic_ValueUpdated;
            }

            if (_device != null && _device.State == DeviceState.Connected)
            {
                await _adapter.DisconnectDeviceAsync(_device);
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Fejl ved disconnect: {ex.Message}");
        }
    }
}