#include "utils.h"

#include <QBluetoothHostInfo>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <BluetoothAPIs.h>
#else
#include <QBluetoothLocalDevice>
#endif


bool Utils::isBluetoothSupported()
{
#ifdef Q_OS_WIN
    BLUETOOTH_FIND_RADIO_PARAMS params;
    ::ZeroMemory(&params, sizeof(params));
    params.dwSize = sizeof(params);

    QList<QBluetoothHostInfo> foundAdapters;

    HANDLE hRadio = nullptr;
    if (const HBLUETOOTH_RADIO_FIND hSearch = ::BluetoothFindFirstRadio(&params, &hRadio)) {
        for (;;) {
            BLUETOOTH_RADIO_INFO radio;
            ::ZeroMemory(&radio, sizeof(radio));
            radio.dwSize = sizeof(radio);

            const DWORD retval = ::BluetoothGetRadioInfo(hRadio, &radio);
            ::CloseHandle(hRadio);

            if (retval != ERROR_SUCCESS)
                break;

            QBluetoothHostInfo adapterInfo;
            adapterInfo.setAddress(QBluetoothAddress(radio.address.ullLong));
            adapterInfo.setName(QString::fromWCharArray(radio.szName));

            foundAdapters << adapterInfo;

            if (!::BluetoothFindNextRadio(hSearch, &hRadio))
                break;
        }

        ::BluetoothFindRadioClose(hSearch);
    }

    return !foundAdapters.empty();
#else
    QBluetoothLocalDevice localDevice;
    return localDevice.isValid();
#endif
}
