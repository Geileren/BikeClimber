using Android.App;
using Android.Content.PM;
using Android.OS;

namespace DataClimber;

[Activity(Theme = "@style/Maui.SplashTheme", MainLauncher = true,
    LaunchMode = LaunchMode.SingleTop,
    ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation |
                           ConfigChanges.UiMode | ConfigChanges.ScreenLayout |
                           ConfigChanges.SmallestScreenSize | ConfigChanges.Density)]
public class MainActivity : MauiAppCompatActivity
{
    protected override void OnCreate(Bundle savedInstanceState)
    {
        base.OnCreate(savedInstanceState);

        var permissions = new string[]
        {
            Android.Manifest.Permission.BluetoothScan,
            Android.Manifest.Permission.BluetoothConnect,
            Android.Manifest.Permission.AccessFineLocation
        };

        RequestPermissions(permissions, 0);
    }
}
