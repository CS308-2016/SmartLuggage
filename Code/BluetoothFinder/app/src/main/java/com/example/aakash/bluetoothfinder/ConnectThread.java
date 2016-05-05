/*
* Team ID :- 8
* Author List :- Paramdeep Singh, Depen Morwani, Aakash Deshpande, Royal Jain
 * Filename :- ConnectThread
 * Theme :- Creates a new thread for connecting with bluetooth device
 * Functions :- isLock, setLock, run, destroyThread, setLock, setUnlock
 * Global variables :- None
 */

package com.example.aakash.bluetoothfinder;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.ParcelUuid;
import android.widget.Toast;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.UUID;
import java.io.IOException;

public class ConnectThread extends Thread {
  //  private final BluetoothSocket mmSocket;
    private final BluetoothDevice mmDevice;
    private final BluetoothAdapter BTAdapter;
    private int unlocked = 0;
    private int open = 0;
    private boolean destroy = false;

    /*
   Function name :- ConnectThread
   Input :- BluetoothDevice, BluetoothAdapter
   Output :- void
   Logic :- Creates a new thread for connecting to bluetooth device
   Example Call :- ConnectThread(device,Adapter)
    */
    public ConnectThread(BluetoothDevice device, BluetoothAdapter lBTAdapter) {
        // Use a temporary object that is later assigned to mmSocket,
        // because mmSocket is final

        BluetoothSocket tmp = null;
        mmDevice = device;
        BTAdapter = lBTAdapter;
        UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");//uuid[0].getUuid();
    }

    /*
   Function name :- isOpen
   Input :- None
   Output :- boolean
   Logic :- checks if the bag is open
   Example Call :- isOpen()
    */
    public boolean isOpen(){
        if (open == 1) return true;
        else return false;
    }

    /*
   Function name :- setopen
   Input :- int
   Output :- void
   Logic :- sets a local variable if the bag is opened
   Example Call :- setopen(a)
    */
    public void setopen(int a){
        open = a;
    }

    /*
   Function name :- run
   Input :- None
   Output :- void
   Logic :- Run function for the thread
   Example Call :- run()
    */
    public void run() {
        // Cancel discovery because it will slow down the connection
        //private BluetoothAdapter BTAdapter = BluetoothAdapter.getDefaultAdapter();
        UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");//uuid[0].getUuid();
        BluetoothSocket tmp2 = null;

        try
        {
            tmp2 = mmDevice.createRfcommSocketToServiceRecord(MY_UUID);
        }
        catch (IOException e1) {
            e1.printStackTrace();
        }

        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            tmp2.connect();
        } catch (IOException connectException)
        {}

        PrintWriter printwriter = null;
        try {
            printwriter = new PrintWriter(tmp2.getOutputStream(), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        printwriter.write("U");
        printwriter.flush();
        printwriter.close();

        try {
            tmp2.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        int i=1;
        while(true) {
            if(destroy){
                System.out.println("Destroyed");
                return;
            }
            BluetoothSocket tmp = null;

            try
            {
                tmp = mmDevice.createRfcommSocketToServiceRecord(MY_UUID);
            }
            catch (IOException e1) {
                e1.printStackTrace();
            }

            BTAdapter.cancelDiscovery();
            try {
                // Connect the device through the socket. This will block
                // until it succeeds or throws an exception
                tmp.connect();
            } catch (IOException connectException)
            {
                System.out.println("Pak gaya" + " " + i + " noob i ");
                // Unable to connect; close the socket and get out
                try
                {
                    System.out.println("" + connectException);
                    tmp.close();
                }
                catch (IOException closeException)
                {

                }

            }

            // Do work to manage the connection (in a separate thread)
            System.out.println("Pairing Done");
            ConnectedThread C = new ConnectedThread(tmp);
            if(unlocked==1){
                C.setunlock();
            }
            else{
                C.setlock();
            }
            C.start();
            try {
                Thread.sleep(1000);
                if(C.isopen()){
                    open = 1;
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            try {
                tmp.close();
                BTAdapter.startDiscovery();
                Thread.sleep(2000);
            } catch (Exception e) {
                e.printStackTrace();
            }
            i = i+1;
        }

    }

    /*
   Function name :- destroy_thread
   Input :- None
   Output :- void
   Logic :- sets a local variable when the thread is destroyec
   Example Call :- destroy_thread()
    */
    public void destroy_thread(){
        destroy = true;
    }

    /*
   Function name :- setunlock
   Input :- None
   Output :- void
   Logic :- sets a local variable if the bag is unlocked
   Example Call :- setunlock()
    */
    public void setunlock(){
        unlocked = 1;
    }

    /*
   Function name :- setlock
   Input :- None
   Output :- void
   Logic :- sets a local variable if the bag is locked
   Example Call :- setlock()
    */
    public void setlock(){
        unlocked = 0;
    }
}