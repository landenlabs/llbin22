#!/bin/csh -f

#
# Android - Stop and Clear app cache via adb with matching package name. 
#
#  Example:
#   ./adb-stop-and-clear.csh  'packageNameorPattern'
#
#  Android 28, does not like the -a on pm list package, so run it with and without
#

set adb="adb"  # default any connected device
set devices=`adb devices | grep '\tdevice' | sed -E 's/\tdevice//'`
set numDevices=`echo $devices | wc -w`
if ($numDevices != 1) then
  echo --- Multiple devices ---
  set devIdx = 1
  foreach dev ($devices) 
     echo "  $devIdx  $dev"
     @ devIdx++
  end
  echo -n "Enter device # (1..$numDevices):"
  set devIdx = $<
  set adb="adb -s $devices[$devIdx]"
  echo "  Adb set to: $adb"
  echo
endif


if ($#argv != 1)  then
  echo "Stop packages matching pattern quoted"
  echo "   ./adb-stop.csh 'twc'"
  echo "   ./adb-stop.csh 'twc.*hm'"
  gecho -e "\nRemember to escape patterns"
  gecho -e "\n=== Example TWC apps ==="
  ($adb shell pm list package && $adb shell pm list package -a) | sort -u | grep -i twc
  exit
endif

set pkgPattern="$1"
 
echo "$adb shell list users"
$adb shell pm list users
 
foreach pkg ( `($adb shell pm list package && $adb shell pm list package -a) | sort -u | grep "$pkgPattern"` )
    echo $pkg
    set dst=`echo $pkg | sed -E 's/.*://'`
    echo "  $adb shell am force-stop all $dst"
    $adb shell am force-stop $dst
    
    echo -n "  $adb shell pm clear -a $dst  "
    $adb shell pm clear -a $dst
    # $adb shell pm clear-app-data $dst
    echo -n "  $adb shell pm clear --user 0 $dst  "
    $adb shell pm clear --user 0 $dst
    echo -n "  $adb shell pm clear --user 10 $dst  "
    $adb shell pm clear --user 10 $dst
    
    # $adb shell pm grant  --user 0 $dst  android.permission.ACCESS_FINE_LOCATION
    # $adb shell pm grant  --user 0 $dst  android.permission.ACCESS_COARSE_LOCATION
    # $adb shell pm grant  --user 0 $dst  android.permission.ACCESS_BACKGROUND_LOCATION

    # https://developer.android.com/about/versions/11/privacy/permissions#dialog-visibility
    # $adb shell pm set-permission-flags $dst android.permission.ACCESS_FINE_LOCATION user-set 
    # $adb shell pm clear-permission-flags $dst android.permission.ACCESS_FINE_LOCATION  user-fixed
    # $adb shell pm set-permission-flags $dst android.permission.ACCESS_COARSE_LOCATION user-set  
    # $adb shell pm clear-permission-flags $dst android.permission.ACCESS_COARSE_LOCATION user-fixed
    # $adb shell pm set-permission-flags $dst android.permission.ACCESS_BACKGROUND_LOCATION user-set 
    # $adb shell pm clear-permission-flags $dst android.permission.ACCESS_BACKGROUND_LOCATION user-fixed
    
    # echo " $adb shell run-as $dst --user 10 ls -alR ."
    # $adb shell run-as $dst --user 10 ls -alR .
    # $adb shell run-as $dst --user 10 rm -rf .
    
    setenv PKG $dst
    ## echo "$adb shell am start -a android.settings.APPLICATION_DETAILS_SETTINGS -d package:${PKG}"
    ## $adb shell am start -a android.settings.APPLICATION_DETAILS_SETTINGS -d package:${PKG}
end

# $adb shell am start -a android.settings.SETTINGS
# $adb shell am start com.android.settings

##     Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
##     intent.setData(Uri.fromParts("package", activity.getPackageName(), null));
##     activity.startActivity(intent);
##
## $adb shell am start -a android.settings.APPLICATION_DETAILS_SETTINGS -d package:${PKG}

gecho -e "\n\n"
