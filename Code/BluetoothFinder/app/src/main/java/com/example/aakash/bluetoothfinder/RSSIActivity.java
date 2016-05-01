/*
* Team ID :- 8
* Author List :- Paramdeep Singh, Depen Morwani, Aakash Deshpande, Royal Jain
 * Filename :- RSSIActivity
 * Theme :- Decides the functioning of mobile application
 * Functions :- onCreate, onDestroy1, receiver
 * Global variables :- None
 */

package com.example.aakash.bluetoothfinder;

import android.graphics.Color;
import android.media.AudioManager;
import android.media.ToneGenerator;
import android.os.Bundle;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;


import java.util.ArrayList;

import static android.app.PendingIntent.getActivity;


public class RSSIActivity extends Activity {

    private ListView mListView;
    private ArrayList<String> mDeviceList = new ArrayList<String>();
    private ArrayAdapter<String> mListAdapter;

    private ListView mListView2;
    private ArrayList<String> mDeviceList2 = new ArrayList<String>();
    private ArrayAdapter mListAdapter2;

    private ArrayList<BluetoothDevice> mDevices = new ArrayList<BluetoothDevice>();
    private BluetoothAdapter BTAdapter = BluetoothAdapter.getDefaultAdapter();
    private BluetoothDevice dev;
    private double[] readings = new double[3];
    private int index=0;
   // private int open = 0;

    private ConnectThread c1;
    private int state = 1;
    private double distance = 0;
    private long prev = 0;
    ToneGenerator toneG = new ToneGenerator(AudioManager.STREAM_ALARM, 100);
    private boolean isinrange = true;

    /*
    Function name :- OnCreate
    Input :- savedInstanceState
    Output :- void
    Logic :- Describes the functioning of app, when it is opened
    Example Call :- onCreate(Bundlea)
     */
    @Override
    public void onCreate(final Bundle savedInstanceState) {
        int REQUEST_ENABLE_BT = 1;
        if (!BTAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rssi);
        mListAdapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, mDeviceList);
        mListAdapter2 = new ArrayAdapter(this, android.R.layout.simple_list_item_1, mDeviceList2);
        mListView = (ListView) findViewById(R.id.listView1);
        mListView.setAdapter(mListAdapter);


        final Button boton = (Button) findViewById(R.id.button1);
        final Button boton3 = (Button) findViewById(R.id.button3);
        View v1 = getLayoutInflater().inflate(R.layout.strength,null);
        registerReceiver(receiver, new IntentFilter(BluetoothDevice.ACTION_FOUND));
        registerReceiver(receiver, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED));

        boton3.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                onDestroy1();
            }
        });

                boton.setOnClickListener(new OnClickListener() {
                    public void onClick(View v) {
                        mDeviceList.clear();
                        mDevices.clear();
                        mListAdapter.notifyDataSetChanged();
                        if (BTAdapter.isDiscovering()) {
                            // cancel the discovery if it has already started
                            BTAdapter.cancelDiscovery();
                        }
                        BTAdapter.startDiscovery();
                    }
                });



        mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position,
                                    long id) {

                dev = mDevices.get(position);
                setContentView(R.layout.device);
                mListView2 = (ListView) findViewById(R.id.listView2);
                mListView2.setAdapter(mListAdapter2);

                mListView2.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position,
                                            long id) {
                        distance = (position*2) + 1;
                        state = 2;
                        c1 = new ConnectThread(dev, BTAdapter);
                        c1.start();

                        setContentView(R.layout.strength);
                        final Button boton2 = (Button) findViewById(R.id.button2);
                        final Button boton4 = (Button) findViewById(R.id.button4);
                        final Button boton5 = (Button) findViewById(R.id.button5);
                        final Button boton6 = (Button) findViewById(R.id.button6);
                        boton4.setOnClickListener(new OnClickListener() {
                            public void onClick(View v) {
                                toneG.stopTone();
                                c1.setopen(0);
                                LinearLayout mealLayout = (LinearLayout) findViewById(R.id.linear);
                                mealLayout.setBackgroundColor(0xFFC5E0DD);
                            }
                        });

                        boton5.setOnClickListener(new OnClickListener() {
                            public void onClick(View v) {
                                c1.setlock();
                            }
                        });

                        boton2.setOnClickListener(new OnClickListener() {
                            public void onClick(View v) {
                                c1.setunlock();
                            }
                        });



                    boton6.setOnClickListener(new OnClickListener() {
                        public void onClick (View v){
                            mDeviceList.clear();
                            mDevices.clear();
                            mDeviceList2.clear();
                            toneG.stopTone();
                            if(c1!=null) c1.destroy_thread();
                            mListAdapter.notifyDataSetChanged();
                            state = 0;
                            prev = 0;
                            Intent intent = getIntent();
                            finish();
                            startActivity(intent);
                            // setContentView(R.layout.activity_rssi);
                            // mListView = (ListView) findViewById(R.id.listView1);
                            // mListView.setAdapter(mListAdapter);
                            distance = 0;
                            if (BTAdapter.isDiscovering()) {
                                // cancel the discovery if it has already started
                                BTAdapter.cancelDiscovery();
                            }
                            BTAdapter.startDiscovery();
                        }
                    });

                    ((TextView) findViewById(R.id.textView2)).setText(dev.getName());
                    if(BTAdapter.isDiscovering())
                    {
                        // cancel the discovery if it has already started
                        BTAdapter.cancelDiscovery();
                    }

                    BTAdapter.startDiscovery();
                }
            });
                        // mListView2.setAdapter(mListAdapter2);
                        mDeviceList2.clear();
                        mDeviceList2.add("1 meters");
                        mDeviceList2.add("3 meters");
                        mDeviceList2.add("5 meters");
                        mListAdapter2.notifyDataSetChanged();
                    }
                });
    }

    /*
   Function name :- OnDestroy1
   Input :- None
   Output :- void
   Logic :- Functioning when app is destroyed
   Example Call :- onDestroy1()
    */
    protected void onDestroy1 (){
        System.out.println("Destroyed1");
        if (BTAdapter.isEnabled()) {
            System.out.println("Destroyed");
            BTAdapter.disable();
        }
       if(c1!=null) c1.destroy_thread();
       // RSSIActivity.this.finish();
        System.exit(0);
        super.onDestroy();

    }

    /*
   Function name :- receiver
   Input :- None
   Output :- BroadcastReceiver
   Logic :- Describes the action to be taken when a new bluetooth device is found
   Example Call :- receiver()
    */
    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if(state==2) {
                LinearLayout mealLayout = (LinearLayout) findViewById(R.id.linear);
                if((System.currentTimeMillis() - prev) > 10000 && prev!=0){
                    System.out.println(System.currentTimeMillis() + " " + prev);
                    //((TextView) findViewById(R.id.textView3)).setBackgroundColor(0xFFFF0000);
                    //getWindow().getDecorView().setBackgroundColor(Color.RED);
                    mealLayout.setBackgroundColor(0xFFAA0022);
                    ((TextView) findViewById(R.id.textView3)).setText("Danger: Device not detected");
                    prev=0;
                    toneG.startTone(ToneGenerator.TONE_DTMF_0, 100000);
                    System.out.println("Device");

                }
                if(c1.isOpen()){
                    //((TextView) findViewById(R.id.textView3)).setBackgroundColor(0xFFFF0000);
                    //getWindow().getDecorView().setBackgroundColor(Color.RED);
                    mealLayout.setBackgroundColor(0xFFAA0022);
                    ((TextView) findViewById(R.id.textView3)).setText("Danger: Bag Opened");
                    toneG.startTone(ToneGenerator.TONE_DTMF_0, 100000);
                    System.out.println("Open");
                }
                //System.out.println(""+state+"ydh");
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                   // System.out.println(""+state+"xdh");
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    //System.out.println("" + device.getName());
                    if (device.getAddress().equals(dev.getAddress())) {
                        prev = System.currentTimeMillis();
                        double rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MIN_VALUE);
                        String name = intent.getStringExtra(BluetoothDevice.EXTRA_NAME);
                        System.out.println("Reading number:  "+index);
                        if(index<2){
                            readings[index] = rssi;
                            index++;
                        }
                        else {
                            readings[index] = rssi;
                            index = 0;
                            double sum = 0;
                            for (double i : readings) {
                                sum += i;
                            }
                            rssi = -sum / 3;
                            double current = 0;
                            double range =0;
                            if(rssi < 65) { current = 0.5; range = 0.25; }
                            else if (rssi < 70) { current = 1; range = 0.5; }
                            else if (rssi < 80) { current = 2; range = 0.5; }
                            else if (rssi < 85) { current = 3; range= 1; }
                            else { current = 5; range = 1; }
                            System.out.println("Distances " + current + " " + distance + " " + rssi);
                            if ((current >= distance) && isinrange) {
                                //((TextView) findViewById(R.id.textView3)).setBackgroundColor(0xFFFF0000);
                                //getWindow().getDecorView().setBackgroundColor(Color.RED);
                               /* View someView = findViewById(R.id.screen);
                                View root = someView.getRootView();
                                root.setBackgroundColor(Color.RED);*/
                                isinrange = false;
                                mealLayout.setBackgroundColor(0xFFAA0022);
                                ((TextView) findViewById(R.id.textView3)).setText("Danger: Device out of range");
                                toneG.startTone(ToneGenerator.TONE_DTMF_0, 100000);
                                System.out.println("Distance");
                            } else  if(current < distance){
                                isinrange = true;
                                ((TextView) findViewById(R.id.textView3)).setBackgroundColor(0xFFC5E0DD);
                                ((TextView) findViewById(R.id.textView3)).setText("Device Distance : " + (current-range)  + "m to " + (current + range) + "m");
                            }
                        }
                        if (BTAdapter.isDiscovering()) {
                            // cancel the discovery if it has already started
                             BTAdapter.cancelDiscovery();
                            }
                            BTAdapter.startDiscovery();
                    }
                }
                else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
                {
                    BTAdapter.startDiscovery();
                }
            }

            else{
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    BluetoothDevice device = intent
                            .getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    mDeviceList.add(device.getName() + "\n" + device.getAddress());
                    mDevices.add(device);
                    mListAdapter.notifyDataSetChanged();

                }

            }
        }
    };
}
