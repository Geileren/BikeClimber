using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using DataClimber.Services;
using System.Diagnostics;

namespace DataClimber.ViewModels;

public partial class DashboardViewModel : ObservableObject
{
    [ObservableProperty]
    private string? latestRawData = "Ingen data endnu...";

    [ObservableProperty]
    private Color iconColor = Colors.Red;

    [ObservableProperty]
    private Color icon2Color = Colors.Blue;

    [ObservableProperty]
    private string statusMessage = "Ikke forbundet";

    private readonly BleService _bleService;

    public DashboardViewModel(BleService bleService)
    {
        _bleService = bleService;
        _bleService.OnDataReceived += UpdateLabel;
        _bleService.OnConnection += UpdateConnection;
        _bleService.OnError += UpdateError;

        Debug.WriteLine("DashboardViewModel initialiseret");
    }

    public async Task StartScanning()
    {
        try
        {
            StatusMessage = "Scanner efter enheder...";
            Debug.WriteLine("Starter BLE scanning...");
            await _bleService.StartScanAsync();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Fejl ved start af scanning: {ex.Message}");
            StatusMessage = $"Fejl: {ex.Message}";
        }
    }

    public void UpdateConnection()
    {
        Debug.WriteLine("UpdateConnection kaldt");
        IconColor = Colors.Green;
        StatusMessage = "Forbundet!";
    }

    private void UpdateError(string error)
    {
        Debug.WriteLine($"BLE fejl: {error}");
        StatusMessage = $"Fejl: {error}";
        IconColor = Colors.Red;
    }

    [RelayCommand]
    void ToggleColor()
    {
        Debug.WriteLine("ToggleColor kommando udført");
        Icon2Color = Icon2Color == Colors.Blue ? Colors.Yellow : Colors.Blue;
    }

    [RelayCommand]
    async Task StartScan()
    {
        await StartScanning();
    }

    private void UpdateLabel(string data)
    {
        Debug.WriteLine($"BLE-data modtaget i ViewModel: '{data}'");
        LatestRawData = data;
        StatusMessage = "Data modtaget!";
    }

    public async Task DisconnectAsync()
    {
        await _bleService.DisconnectAsync();
        IconColor = Colors.Red;
        StatusMessage = "Afbrudt";
    }
}