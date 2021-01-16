//
//  AdvancedSettingsTableViewController.swift
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 6/29/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

/// The view controller for setting the advanced settings.
class AdvancedSettingsTableViewController: UITableViewController {
    
    // Outlets to UI Components
    @IBOutlet weak var labelSamplingFrequency: UILabel!
    @IBOutlet weak var labelWindowSize: UILabel!
    @IBOutlet weak var labelclusteringDecisionRate: UILabel!
    
    
    @IBOutlet weak var maxClusterSize: UILabel!
    @IBOutlet weak var labelChunkSize: UILabel!
    @IBOutlet weak var labelSigma: UILabel!
    @IBOutlet weak var labelFractionRej: UILabel!
    
    //@IBOutlet weak var textFieldWindowSize: UITextField!
    @IBOutlet weak var textFieldStepSize: UITextField!
    @IBOutlet weak var textFieldScreenDispRate: UITextField!
    

    
    //@IBOutlet weak var textNoiseType: UITextField!
    @IBOutlet weak var textClusterSize: UITextField!
    @IBOutlet weak var textChunkSize: UITextField!
    @IBOutlet weak var textSigma: UITextField!
    @IBOutlet weak var textFractionRej: UITextField!
    
    //Add button for newSettings->LoadingButt  , //newSettings->SavingButt
    @IBOutlet weak var switchReading: UISwitch!
    @IBOutlet weak var switchSaving: UISwitch!
    @IBOutlet weak var switchSavingFeat: UISwitch!
    
    // local vars
    var refresh: Timer!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Notify when app enters foreground for updates
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(appActiveAction),
                                               name: NSNotification.Name.UIApplicationDidBecomeActive,
                                               object: UIApplication.shared)
        
        tableView.beginUpdates()
        tableView.endUpdates()
        
        // Only allowed to set if audio not playing
        let notPlaying = !audioController.playAudio
        textFieldStepSize.isEnabled = notPlaying
        textClusterSize.isEnabled   = notPlaying
        textChunkSize.isEnabled     = notPlaying
        textSigma.isEnabled         = notPlaying
        textFractionRej.isEnabled   = notPlaying
        textFieldScreenDispRate.isEnabled = notPlaying
 
        
        // saving/reading switch
        switchSaving.isOn = audioController.LoadingButt
        switchReading.isOn = audioController.SavingButt
        switchSavingFeat.isOn = audioController.SavingFeatButt
        
        // Sampling Frequency
        let samplingFreq: Int = audioController.fs
        labelSamplingFrequency.text = "\(samplingFreq) Hz"
        
        
        // Frame (Window) size
        //let frame: Float = Float(audioController.frameSize)/Float(audioController.fs)
        //textFieldWindowSize.text = String(format: "%.2f", 1000.0 * frame)
        let frame: Float = 2.0 * Float(audioController.stepSize)/Float(audioController.fs)
        labelWindowSize.text = String(format: "%.1f", 1000.0 * frame)
        
        // Step size
        let step: Float = Float(audioController.stepSize)/Float(audioController.fs)
        textFieldStepSize.text = String(format: "%.1f", 1000.0 * step)
        
        // noise type that user is defined
        //let UserNoiseType: String = String(audioController.UserDefinedNoiseType)
        //textNoiseType.text = String(format: "%s", UserNoiseType)
        
        // max cluster size
        let MaxClusterSize: Int = Int(audioController.ClusterSize)
        textClusterSize.text = String(format: "%d", MaxClusterSize)
        
        // Chunk Size
        let ChunkS: Int = Int(audioController.sizeL)
        textChunkSize.text = String(format: "%d", ChunkS)
        
        // Sigma
        let SigmaVal: Double = Double(audioController.sigma)
        textSigma.text = String(format: "%.2f", SigmaVal)
        
        // fraction rejection
        let fractionVal: Double = Double(audioController.fractionRej)
        textFractionRej.text = String(format: "%.2f", fractionVal)
        
        // Show Display update rate
        let displayRate: Float = Float(audioController.DisplayUpdateRate)
        textFieldScreenDispRate.text = String(format: "%.1f", displayRate)
     
        // Show Classification decision update rate
        labelclusteringDecisionRate.text = String(format: "%.1f", 40 * step) // 40 (or actually 39) is decision frame buffer size

        
        // Recognize taps to exit keyboard
        let tapToExitKeyboard: UITapGestureRecognizer =
            UITapGestureRecognizer(target: self,
                                   action: #selector(self.dismissKeyboard))
        view.addGestureRecognizer(tapToExitKeyboard)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
    }
    
    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    
    // Loading switch(Hybrid)
    @IBAction func switchReadingValueChanged(_ sender: UISwitch) {
        audioController.update(LoadingButt: sender.isOn)
        // Refresh with animation based on changed state
        sender.isOn = audioController.LoadingButt
        tableView.beginUpdates()
        tableView.endUpdates()
    }
    
    
    @IBAction func switchSavingValueChanged(_ sender: UISwitch) {
        audioController.update(SavingButt: sender.isOn)
        // Refresh with animation based on changed state
        sender.isOn = audioController.SavingButt
        tableView.beginUpdates()
        tableView.endUpdates()
    }
 
    @IBAction func switchSavingFeatValueChanged(_ sender: UISwitch) {
        audioController.update(SavingFeatButt: sender.isOn)
        // Refresh with animation based on changed state
        sender.isOn = audioController.SavingFeatButt
        tableView.beginUpdates()
        tableView.endUpdates()
    }
    
  
    @IBAction func textFieldStepSizeEditingDidEnd(_ sender: UITextField) {
        let step: Float = Float(audioController.stepSize)/Float(audioController.fs)
        if let text: String = sender.text {
            if let newStepSize: Float = Float(text) {
                if newStepSize <= 0 {
                    sender.text = String(format: "%.1f", 1000.0 * step)
                    showErrorAlert()
                }
                else {
                    audioController.update(stepSize: newStepSize)
                    labelWindowSize.text = String(format: "%.1f", 2.0 * newStepSize)
                    labelclusteringDecisionRate.text = String(format: "%.1f", (40 * newStepSize)/1000)
                    sender.text = String(format: "%.1f", newStepSize)
                }
            }
            else {
                sender.text = String(format: "%.1f", 1000.0 * step)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.1f", 1000.0 * step)
            showErrorAlert()
        }
    }
    
    
    @IBAction func MaxClusterSizeEdittingDidEnd(_ sender: UITextField) {
        // max cluster size
        let MaxClusterSize: Int = Int(audioController.ClusterSize)
        if let text: String = sender.text {
            if let newMaxClusterSize: Int = Int(text) {
                if newMaxClusterSize < 0 {
                    sender.text = String(format: "%d", MaxClusterSize)
                    showErrorAlert()
                }
                else {
                    audioController.update(ClusterSize: newMaxClusterSize)
                    sender.text = String(format: "%d", newMaxClusterSize)
                }
            }
            else {
                sender.text = String(format: "%d", MaxClusterSize)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%d", MaxClusterSize)
            showErrorAlert()
        }

    }
    
    @IBAction func ChunkSizeEditingDidEnd(_ sender: UITextField) {
        let ChunkS: Int = Int(audioController.sizeL)
        if let text: String = sender.text {
            if let newChunkS: Int = Int(text) {
                if newChunkS < 0 {
                    sender.text = String(format: "%d", ChunkS)
                    showErrorAlert()
                }
                else {
                    audioController.update(sizeL: newChunkS)
                    sender.text = String(format: "%d", newChunkS)
                }
            }
            else {
                sender.text = String(format: "%d", ChunkS)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%d", ChunkS)
            showErrorAlert()
        }

    }
    
    
    @IBAction func fractionRejEdittingDidEnd(_ sender: UITextField) {
        let fractionVal: Double = Double(audioController.fractionRej)
        if let text: String = sender.text {
            if let newfractionVal: Double = Double(text) {
                if newfractionVal < 0 {
                    sender.text = String(format: "%.2f", fractionVal)
                    showErrorAlert()
                }
                else {
                    audioController.update(fractionRej: newfractionVal)
                    sender.text = String(format: "%.2f", newfractionVal)
                }
            }
            else {
                sender.text = String(format: "%.2f", fractionVal)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.2f", fractionVal)
            showErrorAlert()
        }
    }
    
    
    @IBAction func SigmaEditingDidEnd(_ sender: UITextField) {
        let SigmaVal: Double = Double(audioController.sigma)
        if let text: String = sender.text {
            if let newSigmaVal: Double = Double(text) {
                if newSigmaVal < 0 {
                    sender.text = String(format: "%.1f", SigmaVal)
                    showErrorAlert()
                }
                else {
                    audioController.update(sigma: newSigmaVal)
                    sender.text = String(format: "%.1f", newSigmaVal)
                }
            }
            else {
                sender.text = String(format: "%.1f", SigmaVal)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.1f", SigmaVal)
            showErrorAlert()
        }
    }
    

   
    //Nasim : adding Target Action Pattern for screen update rate
    @IBAction func textFieldScreenDispRateEdittingDidEnd(_ sender: UITextField) {
        let displayRate: Float = Float(audioController.DisplayUpdateRate)
        if let text: String = sender.text {
            if let newtextScreenDispRate: Float = Float(text) {
                if newtextScreenDispRate < 0 {
                    sender.text = String(format: "%.1f", displayRate)
                    showErrorAlert()
                }
                else {
                    audioController.update(DisplayUpdateRate: newtextScreenDispRate)
                    sender.text = String(format: "%.1f", newtextScreenDispRate)
                }
            }
            else {
                sender.text = String(format: "%.1f", displayRate)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.1f", displayRate)
            showErrorAlert()
        }
    }
    
    
    /// Dismisses the keyboard when the screen is tapped after editing a field.
    func dismissKeyboard() {
        view.endEditing(true) // End editing on tap
    }
    
 
    
    /// Shows an error alert if an error occurs when entering a number.
    ///
    /// - Parameter intRequired: Whether or not an integer is required in the field.
    func showErrorAlert(intRequired: Bool = false, posRequired:Bool = true) {
        let posstr: String = posRequired ? "positive" : "valid"
        let message: String = intRequired ? "Please enter only \(posstr) integer numbers." : "Please enter only \(posstr) numbers."
        let alert: UIAlertController = UIAlertController(title: "Improper Value Entered",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
    }
    
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
    
    /// Shows error if import error when app becomes active.
    ///
    /// Selected to be called so that the imported file is shown.
    func appActiveAction(_ note: NSNotification) {
        if(self.isViewLoaded && self.view.window != nil) {
            // Show error from import if there is one pending
            /*if let importError = importedFileErrorString {
                if(importError != "") {
                    showImportErrorAlert(importError)
                    // Clear error string
                    importedFileErrorString = nil
                }
            }*/
        }
    }
    
}
