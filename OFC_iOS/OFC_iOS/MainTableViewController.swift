//
//  MainTableViewController.swift
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 6/12/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

/// The view controller for the root view of the app.
class MainTableViewController: UITableViewController {
    
    // Outlets to UI components
    @IBOutlet weak var switchOutputType: UISwitch!
    @IBOutlet weak var labelSBL: UILabel!
    @IBOutlet weak var labelStartStop: UILabel!
    @IBOutlet weak var TextVeiwScreen: UITextView!  // Nasim: for results screen
    
    // local vars
    /// The green color for the start button text.
    let startColor: UIColor = UIColor(red: 56.0/256, green: 214.0/256, blue: 116.0/256, alpha: 1.0) // Same as green on switch
    // local vars
    var refresh: Timer!
    var timer:Timer!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Notify when app enters foreground for updates
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(appActiveAction),
                                               name: NSNotification.Name.UIApplicationDidBecomeActive,
                                               object: UIApplication.shared)
        
        // Update LMH at launch with animation
        //updateLMH()
        tableView.beginUpdates()
        tableView.endUpdates()
        
        
        // Set start/stop colors
        startStop(start: audioController.playAudio, setMic: true) // Show stopped or started
        // Calibration and SPL
        refreshSPL()
        
        // Timer to refresh SPL
        refresh = Timer.scheduledTimer(timeInterval: TimeInterval(audioController.dbUpdateInterval),
         target: self,
         selector: #selector(self.refreshSPL),
         userInfo: nil,
         repeats: true)
        
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
    }
    
    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    
    
    /// Starts or stops the audio.
    ///
    /// - Parameters:
    ///   - start: Starts the audio if true, or stops the audio if false.
    ///   - setMic: Enables the microphone input on start if true.
    var isPaused = true;
    func startStop(start: Bool, setMic: Bool) {
        // setMic true for playing back input, false for playing audio from file
        if start {
            labelStartStop.text = "Stop Noise Classification"
            labelStartStop.textColor = UIColor.red
            audioController.update(micStatus: setMic)
            
            audioController.start()

            isPaused = false
            timer = Timer.scheduledTimer(timeInterval: TimeInterval (audioController.DisplayUpdateRate),
                                             target: self,
                                             selector: #selector(self.updateVIEW),
                                             userInfo: nil,
                                             repeats: true)
            
            timer.fire() // start the timer, Causes the timer's message to be sent to its target.
            
   
            
        }
        else {
            labelStartStop.text = "Start Noise Classification"
            labelStartStop.textColor = startColor
            //self.TextVeiwScreen.text = ""
            
            audioController.stop()
            audioController.update(micStatus: false)
            // Set button disables
            //switchOutputType.isEnabled = false
            isPaused = true
            timer = Timer.scheduledTimer(timeInterval: TimeInterval (audioController.DisplayUpdateRate),
                                             target: self,
                                             selector: #selector(self.updateVIEW),
                                             userInfo: nil,
                                             repeats: true)
            timer.invalidate() // Stops the timer from ever firing again and requests its removal from its run loop
            
        }
    }
    
    // @IBAction func refreshButtonStartDidPress(sender: start) { //sender: AnyObject
    // update inforView screen
    func updateVIEW() {
        if ( isPaused == false){
        self.TextVeiwScreen.text = self.TextVeiwScreen.text! + "\n Noise class label " + String(iosAudio.clusterLabel) +
            " out of " + String(iosAudio.totalDetectedClusters)
        + "  classes";
        //    + " - Decision Frame: " + String(iosAudio.frameNumber) ; // + " -- Execution Time: " + String(iosAudio.overallExecutionTime);
        let range = NSMakeRange(self.TextVeiwScreen.text.characters.count - 1, 0)
        self.TextVeiwScreen.scrollRangeToVisible(range)
        print("update is running")
        }
    }
 
    
    //}

    /// Shows an error alert if an error occurs when importing files.
    ///
    /// - Parameter message: The error message to show in the alert.
    func showImportErrorAlert(_ message: String) {
        let alert: UIAlertController = UIAlertController(title: "Import File Error",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
    }
    
 
    /// Updates the gains file label when app becomes active.
    ///
    /// Selected to be called so that the imported file is shown.
    func appActiveAction(_ note: NSNotification) {
        if(self.isViewLoaded && self.view.window != nil) {
            // Show error from import if there is one pending

        }
    }
    

    
    func refreshSPL() {
        labelSBL.text = "\(String(format:"%.2f", audioController.dbpower)) dB SPL"
        //labelCurrentGains.text = "\(audioController.currentGains)"
    }
    
    // Selecting row which functions as a button, we want to make Start as button. The indexing start from 0.
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if indexPath.section == 1 && indexPath.row == 0 {
            startStop(start: !audioController.playAudio, setMic: true) // If it's playing, stop it; if not, start it
            tableView.deselectRow(at: indexPath, animated: true)
        }
    }
    
}
