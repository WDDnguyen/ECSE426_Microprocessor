/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.amazonaws.demo.s3transferutility;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;

import com.amazonaws.demo.s3transferutility.SampleGattAttributes;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

import static java.lang.Math.atan;
import static java.lang.Math.sqrt;


//Service for managing connection and data communication with a GATT server hosted on a given Bluetooth LE device.

public class BluetoothLeService extends Service {
    private final static String TAG = BluetoothLeService.class.getSimpleName();

    File root;
    File filepath;
    private boolean empty = true;
    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mConnectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public final static String ACTION_GATT_CONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE =
            "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA =
            "com.example.bluetooth.le.EXTRA_DATA";

    public static final UUID ServiceUUID = UUID.fromString("02366e80-cf3a-11e1-9ae9-0012a5d5c51b");

    public static final UUID r1 = UUID.fromString("E13E78A0-CF4A-11E1-8FFC-0002A5D5C51B");
    public static final UUID r2 = UUID.fromString("E23E78A0-CF4A-11E1-8FFC-0002A5D5C51B");
    public static final UUID r3 = UUID.fromString("E33E78A0-CF4A-11E1-8FFC-0002A5D5C51B");
    public static final UUID r4 = UUID.fromString("E43E78A0-CF4A-11E1-8FFC-0002A5D5C51B");
    public static final UUID r5 = UUID.fromString("E53E78A0-CF4A-11E1-8FFC-0002A5D5C51B");
    public static final UUID r6 = UUID.fromString("e63e78a0-cf4a-11e1-8ffc-0002a5d5c51b");

    // WRITE UUID
    public static final UUID w1 = UUID.fromString("01C50B60-E48C-11E2-A073-0002A5D5C51B");

    private List<BluetoothGattCharacteristic> characteristicList;

    public byte[] byteArray = new byte[10];
    public int offset = 0;
    public int sendOffset = 0;
    public int[] pitch = new int[250];
    public int[] roll = new int[250];

    //concatenated short after concatenating.
    private int[] x = new int[250];
    private int[] y = new int[250];
    private int[] z = new int[250];

    private byte[] xu = new byte[250];
    private byte[] xl = new byte[250];

    private byte[] yu = new byte[250];
    private byte[] yl = new byte[250];

    private byte[] zu = new byte[250];
    private byte[] zl = new byte[250];

    private byte[] sentToW1 = new byte[10];

    private byte[] rollByte = new byte[250];
    private byte[] pitchByte = new byte[250];


    private byte[] dummySend = new byte[250];

    private short concatenatedByte;

    private int serviceIndex;

    int lastUUID = -1;

    private String uuid;

    // Implements callback methods for GATT events that the app cares about such as connection change and services discovered.

    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

        // when the app is connected to a gatt  then the connection is changed which trigger this function on the callback
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String intentAction;
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                intentAction = ACTION_GATT_CONNECTED;
                mConnectionState = STATE_CONNECTED;
                // when the profile is connected then proceed to broadcast the status of the gatt to the main activity
                broadcastUpdate(intentAction);
                Log.i(TAG, "Connected to GATT server.");
                // Attempts to discover services after successful connection.
                Log.i(TAG, "Attempting to start service discovery:" +
                        mBluetoothGatt.discoverServices());

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                intentAction = ACTION_GATT_DISCONNECTED;
                mConnectionState = STATE_DISCONNECTED;
                Log.i(TAG, "Disconnected from GATT server.");
                // if profile is disconnected then broadcast the status of the gatt to the main activity.
                broadcastUpdate(intentAction);
            }
        }

        // new services discovered from GATT will update the list of services

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {

            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
                offset = 0;
            } else {
                Log.w(TAG, "onServicesDiscovered received: " + status);
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
                getCharacteristicValue(characteristic);
            }
        }

        // when 6th read character changes
        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            Log.d(TAG,"VALUE IS CHANGING");
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    // change broadcast when event occured
    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        // send broadcast to main Activity receiver to communicate gatt condition
        sendBroadcast(intent);
    }

    private void getCharacteristicValue(final BluetoothGattCharacteristic characteristic){

        byte[] data = characteristic.getValue();
        Log.d(TAG," :DATA LENGTH " + data.length);
        if (data == null){
            Log.d(TAG," :INVALID DATA LENGTH ");
            return;
        }

        uuid = characteristic.getUuid().toString();
        Log.d(TAG, "offset : " + offset);

        if (uuid.equals(r1.toString())) {
            Log.d(TAG,"adding bytes to r1");
            for(int i = 0 ; i < data.length; i++){
                xu[offset + i] = data[i];
            }

        }
        else if (uuid.equals(r2.toString())){
            Log.d(TAG,"adding bytes to r2");
            for(int i = 0; i < data.length; i++){
                xl[offset + i] = data[i];
            }

        }
        else if (uuid.equals(r3.toString())){
            Log.d(TAG,"adding bytes to r3");
            for(int i = 0; i < data.length; i++){
                yu[offset + i] = data[i];
            }

        }
        else if (uuid.equals(r4.toString())){
            Log.d(TAG,"adding bytes to r4");
            for(int i = 0; i < data.length; i++){
                yl[offset + i] = data[i];
            }

        }
        else if (uuid.equals(r5.toString())){
            Log.d(TAG,"adding bytes to r5");
            for(int i = 0; i < data.length; i++){
                zu[offset + i] = data[i];
            }

        }
        else if (uuid.equals(r6.toString())){
            Log.d(TAG,"adding bytes to r");
            for(int i = 0; i < data.length; i++){
                zl[offset + i] = data[i];
            }

        }

    }

    //Overloading function

    private void broadcastUpdate(final String action, final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);

        int flag = characteristic.getProperties();
        int format = -1;
        //format = BluetoothGattCharacteristic.FORMAT_UINT8;
        //Log.d(TAG,"Characteristic of ACC.");

        // For all other profiles, writes the data formatted in HEX.
        final byte[] data = characteristic.getValue();
        //Log.d(TAG,"THIS SHOULD BE 10" + data.length);
        if (data != null && data.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(data.length);
            for(byte byteChar : data)

                stringBuilder.append(String.format("%02X ", byteChar));
                intent.putExtra(EXTRA_DATA, new String(data) + "\n" + stringBuilder.toString());
        }


        sendBroadcast(intent);
    }

    public class LocalBinder extends Binder {
        BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        // After using a given device, you should make sure that BluetoothGatt.close() is called
        // such that resources are cleaned up properly.  In this particular example, close() is
        // invoked when the UI is disconnected from the Service.
        close();
        return super.onUnbind(intent);
    }

    private final IBinder mBinder = new LocalBinder();

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter through
        // BluetoothManager.
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Log.e(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }

        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
            return false;
        }

        return true;
    }

    /**
     * Connects to the GATT server hosted on the Bluetooth LE device.
     *
     * @param address The device address of the destination device.
     *
     * @return Return true if the connection is initiated successfully. The connection result
     *         is reported asynchronously through the
     *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     *         callback.
     */
    public boolean connect(final String address) {
        if (mBluetoothAdapter == null || address == null) {
            Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }

        // Previously connected device.  Try to reconnect.
        if (mBluetoothDeviceAddress != null && address.equals(mBluetoothDeviceAddress)
                && mBluetoothGatt != null) {
            Log.d(TAG, "Trying to use an existing mBluetoothGatt for connection.");
            if (mBluetoothGatt.connect()) {
                mConnectionState = STATE_CONNECTING;
                return true;
            } else {
                return false;
            }
        }

        // search the device from the bluetooth adapter
        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (device == null) {
            Log.w(TAG, "Device not found. Unable to connect.");
            return false;
        }
        // We want to directly connect to the device, so we are setting the autoConnect
        // parameter to false.
        mBluetoothGatt = device.connectGatt(this, false, mGattCallback);
        Log.d(TAG, "Trying to create a new connection.");
        mBluetoothDeviceAddress = address;
        mConnectionState = STATE_CONNECTING;
        return true;
    }

    /**
     * Disconnects an existing connection or cancel a pending connection. The disconnection result
     * is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */

    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.disconnect();
    }

    /**
     * After using a given BLE device, the app must call this method to ensure resources are
     * released properly.
     */
    public void close() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * Enables or disables notification on a give characteristic.
     *
     * @param characteristic Characteristic to act on.
     * @param enabled If true, enable notification.  False otherwise.
     */
    public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic,
                                              boolean enabled) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);

    }

    /**
     * Retrieves a list of supported GATT services on the connected device. This should be
     * invoked only after {@code BluetoothGatt#discoverServices()} completes successfully.
     *
     * @return A {@code List} of supported services.
     */
    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null) return null;

        return mBluetoothGatt.getServices();
    }

    public BluetoothGattService getGattService(UUID uuid){
        if (mBluetoothGatt == null) return null;

        return mBluetoothGatt.getService(uuid);
    }

    public void writeCharacteristic(byte[] send, UUID uuid, BluetoothGattService service) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        if (service == null) {
            Log.e(TAG, "service not found!");
            return;
        }

        // get the read characteristic from the service
        BluetoothGattCharacteristic mWriteCharacteristic = service.getCharacteristic(uuid);
        if (mWriteCharacteristic == null){
            Log.e(TAG, "characteristic not found");
            return;
        }

        mWriteCharacteristic.setValue(send);
        if(mBluetoothGatt.writeCharacteristic(mWriteCharacteristic) == false){
            Log.w(TAG, "Failed to write characteristic");
        }
        else {
            Log.d(TAG,"Sucessfully written to characteristic" + uuid.toString());
        }
    }

    public short twoBytesToShort(byte b1, byte b2) {
        return (short) ((b1 << 8) | (b2 & 0xFF));
    }

    public void concatenateTwoCharacteristics(byte[] b1, byte[] b2, int[] coord){
        for(int i = 0 ; i < 250; i++){
            coord[i] = twoBytesToShort(b1[i],b2[i]);
        }
    }

    public void printByteArray(){
        Log.d(TAG, "xu : " + Arrays.toString(xu));
        Log.d(TAG, "xl : " + Arrays.toString(xl));
        Log.d(TAG, "yu : " + Arrays.toString(yu));
        Log.d(TAG, "yl : " + Arrays.toString(yl));
        Log.d(TAG, "zu : " + Arrays.toString(zu));
        Log.d(TAG, "zl : " + Arrays.toString(zl));
    }

    public void printIntArray(){
        Log.d(TAG, " x : " + Arrays.toString(x) + "\n" + " y : " + Arrays.toString(x) + "\n" + " z : " + Arrays.toString(x));
    }

    public void printCalculation(){
        Log.d(TAG, "roll : " + Arrays.toString(roll) + "\n" + " pitch : " + Arrays.toString(pitch));

    }

    public void calculateRoll(){
        double val = 180.0/Math.PI;
        double rollCalculation;
        for(int i =0; i < 250; i++){
            rollCalculation = (x[i]/ Math.sqrt( y[i] * y[i] + z[i] * z[i]));
            roll[i] = (int) (Math.atan(rollCalculation) * val) ;
        }
    }

    public void calculatePitch(){
        double val = 180.0/Math.PI;
        double pitchCalculation;
        for(int i =0; i < 250; i++){
            pitchCalculation = (y[i]/ Math.sqrt( x[i] * x[i] + z[i] * z[i]));
            Log.d(TAG, "calculation : " + pitchCalculation);
            pitch[i] = (int) (Math.atan(pitchCalculation) * val);
            Log.d(TAG, "calculation of pitch : " + pitch[i]);
        }
    }

    private void writeToFile(int[] roll, int[] pitch){

        try {
            root = new File(Environment.getExternalStorageDirectory(),"uP");
            if (!root.exists()) {
                root.mkdirs(); // create folder
            }

            filepath = new File(root, "accelerometer.txt"); // file path to save
            FileWriter writer = new FileWriter(filepath, true);

            for(int i = 0 ; i < 250; i++){
                writer.write("roll : " + roll[i] + " pitch : " + pitch[i] + "\n");
            }

            writer.flush();
            writer.close();

        } catch (IOException e){
        }
    }

    public void writeDummy(BluetoothGattService service, UUID chairUUID){
        for (int i =0; i < 250; i++){
            dummySend[i] = 1;
        }
        writeCharacteristic(dummySend, chairUUID , service);
    }

    public void convertRoll(){
        for(int i = 0; i < 250; i++){
            rollByte[i] = (byte) roll[i];
        }
    }

    public void convertPitch(){
        for(int i = 0; i < 250; i++){
            pitchByte[i] = (byte) pitch[i];
        }
    }

    public void sendDataToWriteCharacteristics(String data, BluetoothGattService service){
        Log.d(TAG, "SENDING VALUE");
        if (data.equals("pitch")){
            Log.d(TAG,"ITS A PITCH");

            for(int i =0; i < 10 ; i++){
                sentToW1[i] = pitchByte [i + sendOffset];
            }
            writeCharacteristic(sentToW1, w1, service);
        }

        else if (data.equals("roll")) {
            Log.d(TAG,"ITS A ROLL");
            for(int i =0; i < 10 ; i++){
                sentToW1[i] = rollByte [i + sendOffset];
            }
            writeCharacteristic(sentToW1, w1, service);
        }
    }

    public void acquireValuesFromCharacteristics(String coord){
        int iteration = 0;
        List<BluetoothGattService>  service = mBluetoothGatt.getServices();
        Log.d(TAG, "size of service list=" + service.size());

        for(int i =0; i < service.size(); i++) {
            if ((service.get(i).getUuid().toString()).equals(ServiceUUID.toString())) {
                serviceIndex = i;
                characteristicList = service.get(i).getCharacteristics();
                Log.d(TAG, "Size of gatt characteristic " + characteristicList.size());
                String uuid = "";

                //add first 10 bytes of each characteristics for read
                iteration = characteristicList.size();
                while (offset < 250) {
                    for (i = 0; i < iteration; i++) {
                        Log.d(TAG, "LOOPING INSIDE SERVICE DISCOVERED MULTIPLE TIMES");
                        if (i < 6) {
                            Log.d(TAG, "ACQUIRING DATA FOR UUID : " + characteristicList.get(i).getUuid().toString());

                            mBluetoothGatt.readCharacteristic(characteristicList.get(i));
                            try {
                                Thread.sleep(500);
                            } catch (InterruptedException e) {
                            }


                        }
                    }
                    offset =  offset + 10;
                }
                printByteArray();

                concatenateTwoCharacteristics(xu,xl,x);
                concatenateTwoCharacteristics(yu,yl,y);
                concatenateTwoCharacteristics(zu,zl,z);

                printIntArray();

                calculatePitch();
                calculateRoll();
                printCalculation();

                //need to write to file then upload to aws
                writeToFile(roll,pitch);

                //start writing characteristic the values back to the ble device
                offset = 0;

                convertPitch();
                convertRoll();

                for(int k =0; k < 25; k++){
                    Log.d(TAG, "trying to write data");
                    sendDataToWriteCharacteristics(coord,mBluetoothGatt.getService(ServiceUUID));
                    sendOffset = sendOffset + 10;
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                    }
                }

                sendOffset = 0;

            }
        }
    }

}
