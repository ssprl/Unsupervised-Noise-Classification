package com.superpowered.NCAndroid;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Button;
import java.io.File;
import java.util.ArrayList;
import java.util.Date;

import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    //SeekBar samplingRate;// quietAdjustment;
    Switch storeFeaturesSwitch, SavingClassificationDataSwitch, HybridClassificationSwitch; //playAudioSwitch
    EditText DecisionRateVal,  sigmaVal ,fractionRejVal , ChunkSizeVal ; //decisionRate,
    Button startButton, stopButton;// readFileButton;
    String fileName,
            folderName = Environment.getExternalStorageDirectory().toString() + "/OFC_Android/";
    String HybridFileName; // for hybrid classification
    String samplerateString = null, buffersizeString = null;
    TextView statusView;
    SharedPreferences prefs;
    SharedPreferences.Editor prefEdit;
    public static final String appPreferences = "appPrefs";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initializeIds();
        prefs = PreferenceManager.getDefaultSharedPreferences(this);
        prefEdit = prefs.edit();
        loadUserDefaults();

        enableButtons();

        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
            Log.d("Sampling Rate",audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE));
            Log.d("Frame Size",audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER));
        }

        File folder = new File(folderName);
        if(!folder.exists()){
            folder.mkdirs();
        }
    }

    public void setUserDefaults(){

        if(prefs.getBoolean("Initialized", false)) {

            prefEdit.putString("Decision Rate", DecisionRateVal.getText().toString());
            prefEdit.putBoolean("Hybrid Classification", HybridClassificationSwitch.isChecked());
            prefEdit.putBoolean("Saving Classification Data", SavingClassificationDataSwitch.isChecked());
            prefEdit.putBoolean("Saving Extracted Features", storeFeaturesSwitch.isChecked());
            prefEdit.putString("Sigma",sigmaVal.getText().toString());
            prefEdit.putString("Fraction Rejection", fractionRejVal.getText().toString());
            prefEdit.putString("#Decisions in Chunk", ChunkSizeVal.getText().toString());
            prefEdit.apply();
        }
        else {
            prefEdit.putBoolean("Initialized", true);
            prefEdit.putString("Decision Rate", "0.50");
            prefEdit.putBoolean("Hybrid Classification", false);
            prefEdit.putBoolean("Saving Classification Data", false);
            prefEdit.putBoolean("Saving Extracted Features", false);
            prefEdit.putString("Sigma", "2.0");
            prefEdit.putString("Fraction Rejection", "0.01");
            prefEdit.putString("#Decisions in Chunk", "10");
            prefEdit.apply();
        }
    }
    public void loadUserDefaults(){
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        if(prefs.getBoolean("Initialized", false)) {
            DecisionRateVal.setText(prefs.getString("Decision Rate", "0.50"));
            HybridClassificationSwitch.setChecked(prefs.getBoolean("Hybrid Classification", false));
            SavingClassificationDataSwitch.setChecked(prefs.getBoolean("Saving Classification Data", false));
            storeFeaturesSwitch.setChecked(prefs.getBoolean("Saving Extracted Features", false));
            sigmaVal.setText(prefs.getString("Sigma", "2.0"));
            fractionRejVal.setText(prefs.getString("Fraction Rejection", "0.01"));
            ChunkSizeVal.setText(prefs.getString("#Decisions in Chunk", "10"));
        }
    }

    public void enableButtons(){
        HybridClassificationSwitch.setEnabled(true);
        SavingClassificationDataSwitch.setEnabled(true);
        storeFeaturesSwitch.setEnabled(true);
        DecisionRateVal.setEnabled(true);
        startButton.setEnabled(true);
        stopButton.setEnabled(true);
        sigmaVal.setEnabled(true);
        fractionRejVal.setEnabled(true);
        ChunkSizeVal.setEnabled(true);
    }

    public void disableButtons(){
        HybridClassificationSwitch.setEnabled(false);
        SavingClassificationDataSwitch.setEnabled(false);
        storeFeaturesSwitch.setEnabled(false);
        DecisionRateVal.setEnabled(false);
        startButton.setEnabled(false);
        stopButton.setEnabled(false);
        sigmaVal.setEnabled(false);
        fractionRejVal.setEnabled(false);
        ChunkSizeVal.setEnabled(false);
    }

    public void initializeIds(){
        HybridClassificationSwitch = (Switch) findViewById(R.id.switchHybridClassification);
        SavingClassificationDataSwitch = (Switch) findViewById(R.id.switchSavingClassificationData);
        storeFeaturesSwitch = (Switch) findViewById(R.id.switchStoreFeatures);
        DecisionRateVal = (EditText) findViewById(R.id.DecisionRateVal);
        startButton = (Button) findViewById(R.id.buttonStart);
        stopButton = (Button) findViewById(R.id.buttonStop);
        statusView = (TextView) findViewById(R.id.statusView);
        sigmaVal = (EditText) findViewById(R.id.sigmaVal);
        fractionRejVal = (EditText) findViewById(R.id.fractionRejVal);
        ChunkSizeVal = (EditText) findViewById(R.id.ChunkSizeVal);
    }

    public void onStartClick(View view) {

        setUserDefaults();
        disableButtons();
        int bufferSize; //should be 600
        int StepSize = 25;
        float decisionBufferLength = ((Float.parseFloat(DecisionRateVal.getText().toString()) * 1000)/(StepSize/2)); //for decision rate of 0.5 sec, should be 40
        bufferSize = (int) ((48000 * StepSize)/(2*1000)); //getSamplingRate(), Float.parseFloat(OverlapSizeVal.getText().toString()
        statusView.append("\nRecording Started\n");

        if(storeFeaturesSwitch.isChecked()){
            fileName = folderName + new SimpleDateFormat("yyyy_MM_dd_HH_mm_ss").format(new Date()) + ".txt";
            Log.d("Filename",fileName);
        }

        if(HybridClassificationSwitch.isChecked() | SavingClassificationDataSwitch.isChecked()){
            HybridFileName = folderName + "Hybrid_clusterParameters" + ".dat";
            Log.d("Filename",HybridFileName);
        }
        // Get the device's sample rate and buffer size to enable low-latency Android audio output, if available.
        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
            samplerateString = Integer.toString(48000);//audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            buffersizeString = Integer.toString(bufferSize); //audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        }

        if (samplerateString == null) samplerateString = "44100";
        if (buffersizeString == null) buffersizeString = "512";

        System.loadLibrary("FrequencyDomain");
        FrequencyDomain(48000,//Integer.parseInt(samplerateString),
                Integer.parseInt(buffersizeString),
                2,      //Float.parseFloat(decisionRate.getText().toString()),
                90,  //(float) (quietAdjustment.getProgress()),
                false, //playAudioSwitch.isChecked(),
                storeFeaturesSwitch.isChecked(),
                fileName,
                Double.parseDouble(sigmaVal.getText().toString()),
                Double.parseDouble(fractionRejVal.getText().toString()),
                HybridClassificationSwitch.isChecked(),
                SavingClassificationDataSwitch.isChecked(),
                decisionBufferLength,
                Integer.parseInt(ChunkSizeVal.getText().toString()),
                HybridFileName);

        handler.postDelayed(r,1000);
        stopButton.setEnabled(true); // added by Nasim
    }

    public void onStopClick(View view) {
        enableButtons();
        statusView.append("\n Recording Stopped\n");
        System.loadLibrary("FrequencyDomain");
        StopAudio(fileName);
        handler.removeCallbacks(r);
    }

    public void getTime(){

        statusView.setMovementMethod(new ScrollingMovementMethod());
        if (getClusterLabel() >= 0 & getClusterLabel() <30){
            statusView.append(
                    //"Frame Processing Time: " +
                    //new DecimalFormat("##.##").format(getExecutionTime()) +
                    //" ms\nDetected Class:" + getDetectedClassLabel() +
                    "Detected cluster: " + getClusterLabel() + " out of " + getTotalClusters() + " clusters." +
                            " | dB Power: " + new DecimalFormat("##").format(getdbPower()) + " dBFS\n"
                    //+ "********************** \n"
            );
        }
        else {
            statusView.append(
                    //"Frame Processing Time: " +
                    //new DecimalFormat("##.##").format(getExecutionTime()) +
                    //" ms\nDetected Class:" + getDetectedClassLabel() +
                    "Detected cluster: " + 0 + " out of " + getTotalClusters() + " clusters." +
                            " | dB Power: " + new DecimalFormat("##").format(getdbPower()) + " dBFS\n"
                    //+ "********************** \n"
            );
        }
        final int scrollAmount = statusView.getLayout().getLineTop(statusView.getLineCount()) - statusView.getHeight();
        if (scrollAmount > 0)
            statusView.scrollTo(0, scrollAmount);
        else
            statusView.scrollTo(0, 0);
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    Handler handler = new Handler();

    final Runnable r = new Runnable() {
        @Override
        public void run() {
            getTime();
            handler.postDelayed(this, 1000);
        }
    };


    public static String getExtensionOfFile(String name)
    {
        String fileExtension="";

        // If fileName do not contain "." or starts with "." then it is not a valid file
        if(name.contains(".") && name.lastIndexOf(".")!= 0)
        {
            fileExtension=name.substring(name.lastIndexOf(".")+1);
        }

        return fileExtension;
    }


    //Native function declaration
    private native void FrequencyDomain(int samplerate,
                                        int buffersize,
                                        float decisionRate,
                                        float quietThreshold,
                                        boolean playAudio,
                                        boolean storeFeatures,
                                        String fileName,
                                        double sigma,
                                        double fractionRej,
                                        boolean HybridButton,
                                        boolean SavingClassifDataButton,
                                        float decisionBufferLength,
                                        int ChunkSize,
                                        String HybridFileName);
    private native void StopAudio(String fileName);
    private native float getExecutionTime();
    private native int getClusterLabel ();
    private native int getTotalClusters();
    private native float getdbPower();

}
