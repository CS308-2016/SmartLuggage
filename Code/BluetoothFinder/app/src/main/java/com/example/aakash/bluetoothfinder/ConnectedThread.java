/*
* Team ID :- 8
* Author List :- Paramdeep Singh, Depen Morwani, Aakash Deshpande, Royal Jain
 * Filename :- ConnectedThread
 * Theme :- Sends a Lock or Unlock signal based on status and reads from BT module
 * Functions :- isOpen, setState, run, destroyThread, check, setLock, setUnlock
 * Global variables :- None
 */

package com.example.aakash.bluetoothfinder;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.UUID;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class ConnectedThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private PrintWriter printwriter;
    private int state=1;
    private boolean destroy = false;
    private boolean unlock = false;
   // private BtTransmission btTransmit;

    /*
   Function name :- ConnectedThtread
   Input :- Bluetoothsocket
   Output :- void
   Logic :- Constructor
   Example Call :- ConnectedThread(socket)
    */
    public ConnectedThread(BluetoothSocket socket) {
        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
           // btTransmit = new BtTransmission();
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { System.out.println(e);}

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    /*
   Function name :- isopen
   Input :- None
   Output :- void
   Logic :- checks if the bag is in open state
   Example Call :- isopen()
    */
    public boolean isopen(){
        if(state == 2){
            return true;
        }
        return false;
    }

    /*
   Function name :- setstate
   Input :- int
   Output :- void
   Logic :- sets a local variable according to whether the bag is locked or unlocked
   Example Call :- setstate(1)
    */
    public void setstate(int a){
        state = a;
    }

    /*
   Function name :- run
   Input :- None
   Output :- void
   Logic :- Run function for the thread
   Example Call :- run()
    */
    public void run() {
        byte[] buffer = new byte[1024];  // buffer store for the stream
        int bytes; // bytes returned from read()
        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                if(destroy){
                    return;
                }
                printwriter = null;
                try {
                    printwriter = new PrintWriter(mmSocket.getOutputStream(), true);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if(unlock) {

                    printwriter.write("U");
                    System.out.println("Unlock");
                }
                else{
                    printwriter.write("L");
                    System.out.println("Lock");
                }
                printwriter.flush();

                bytes = mmInStream.read(buffer);

                // Send the obtained bytes to the UI activity
                System.out.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" + buffer);

                boolean Unlocked = check(buffer);

                if(!Unlocked){
                    state=2;
                    return;
                }

            //mHandler.obtainMessage(MESSAGE_READ, bytes, -1, buffer).sendToTarget();
            } catch (Exception e) {
                System.out.println("mmSocket error: "+e);
                break;
            }
        }
    }

    /*
  Function name :- destroy_thread
  Input :- None
  Output :- void
  Logic :- Functioning when the thread is destoryed
  Example Call :- destroy_thread()
   */
    public void destroy_thread(){
        destroy = true;
    }


    /*
  Function name :- check
  Input :- Byte[]
  Output :- boolean
  Logic :- Check if the input contains "O" for checking open sent from device
  Example Call :- check(bytes)
   */
    public boolean check(byte[] bytes) {
        if (bytes.toString().contains("O")) {
            return false;
        } else if (bytes.toString().contains("C")) {
            return true;
        } else {
            System.out.println("Error message: " + bytes.toString());
            return false;
        }
    }

    /*
  Function name :- setunlock
  Input :- None
  Output :- void
  Logic :- Functioning when bag is unlocked
  Example Call :- setunlock()
   */
    public void setunlock(){
        unlock = true;
    }

    /*
  Function name :- setlock
  Input :- None
  Output :- void
  Logic :- Functioning when bag is locked
  Example Call :- setlock()
   */
    public void setlock(){
        unlock = false;
    }

}