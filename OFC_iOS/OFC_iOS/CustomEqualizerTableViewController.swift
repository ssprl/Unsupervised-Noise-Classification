//
//  CustomEqualizerTableViewController.swift
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 7/2/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

/// The view controller for setting custom gains for the equalizer.
class CustomEqualizerTableViewController: UITableViewController {
    
    // Outlets to UI Components
    @IBOutlet weak var segmentedControlLMH: UISegmentedControl!
    @IBOutlet weak var labelLMHDesc: UILabel!
    @IBOutlet weak var sliderGains1: UISlider!
    @IBOutlet weak var sliderGains2: UISlider!
    @IBOutlet weak var sliderGains3: UISlider!
    @IBOutlet weak var sliderGains4: UISlider!
    @IBOutlet weak var sliderGains5: UISlider!
    @IBOutlet weak var labelResetThisGains: UILabel!
    
    // local vars
    /// The maximum value of the gains sliders
    let SLIDER_MAX: Float = 2.0
    /// The minimum value of the gains sliders.
    let SLIDER_MIN: Float = 0.0
    /// A store of the initial position of the sliders, to see if they have been changed.
    var initGains: [Float]!
    /// A convenience getter for all of the gains sliders as an array of sliders.
    var sliders: [UISlider] {
        get {
            return [sliderGains1,
                    sliderGains2,
                    sliderGains3,
                    sliderGains4,
                    sliderGains5]
        }
    }
    /// A convenience getter and setter for the gains to use in an update.
    /// 
    /// Note that gain values outside of the range of the sliders are handled here.
    var sliderGains: [Float] {
        get {
            let audioGains: [Float] = audioController.gains
            let sliders: [UISlider] = self.sliders
            var toReturn: [Float] = Array(repeating: 0.0, count: sliders.count)
            for i:Int in 0...sliders.count - 1 {
                if initGains[i] == sliders[i].value {
                    // Slider value hasn't changed
                    toReturn[i] = audioGains[i]
                }
                else {
                    // Slider value has changed
                    toReturn[i] = sliders[i].value
                }
            }
            return toReturn
        }
        set {
            // Set to SLIDER_MIN if below min, to SLIDER_MAX if above max, or to new value if in range
            let toSet = newValue.map({
                (val: Float) -> Float in
                if val < SLIDER_MIN {
                    return SLIDER_MIN
                }
                else if val > SLIDER_MAX {
                    return SLIDER_MAX
                }
                else {
                    return val
                }
            })
            let sliders: [UISlider] = self.sliders
            for i:Int in 0...sliders.count - 1 {
                sliders[i].value = toSet[i]
            }
        }
    }
    /// For storing whether the gains were being automatically switched before entering this view
    var autoGains: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Notify when app enters foreground for updates
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(appActiveAction),
                                               name: NSNotification.Name.UIApplicationDidBecomeActive,
                                               object: UIApplication.shared)
        
        // Store whether gains were being automatically switched, change back after done making custom equalizer
        autoGains = audioController.autoGains
        audioController.update(autoGains: false)
        
        // Set current gains to LMH
        updateLMH()
        
        // Update reset label
        labelResetThisGains.text = "Reset \(audioController.currentGains) Gains to Default"
        
        // LMH Description
        labelLMHDesc.text = "Low is \(audioController.lowDef) dB SPL, " +
                            "Medium is \(audioController.mediumDef) dB SPL, and " +
                            "High is \(audioController.highDef) dB SPL"
        
        // Set gains for all sliders (AudioSettingsController guarantees they exist)
        sliderGains = audioController.gains
        initGains = sliders.map({$0.value})
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        // Reset auto gains to previous value
        audioController.update(autoGains: autoGains)
        
        super.viewWillDisappear(animated)
    }
    
    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    
    @IBAction func barButtonItemSaveAsPressed(_ sender: UIBarButtonItem) {
        // First, alert user with file name
        let alert: UIAlertController = UIAlertController(title: "Save Gains",
                                                         message: "Enter a name to save these gains in a file.",
                                                         preferredStyle: .alert)
        alert.addTextField { (fileNameBox) in
            fileNameBox.text = ""
        }
        let ok = UIAlertAction(title: "Save", style: .default, handler: {
            [weak alert] (_) in
            // Format name as JSON, edit name, make sure availible,
            if let textField = alert?.textFields?[0] {
                // This should always happen, since textField was added
                if let fileName: String = textField.text {
                    if fileName != "" && !fileName.contains("/") {
                        // Save gains file
                        var errorString: String = ""
                        if let saveName: String = GainsFiles.saveGainsFile(fileName,
                                                                        numGains: audioController.numGains,
                                                                        LMHGains: audioController.LMHGains,
                                                                        errorString: &errorString) {
                            // Once written, set file name as file name in audioController
                            audioController.gainsFile = saveName
                        }
                        else {
                            self.showErrorAlert(errorString)
                        }
                    }
                    else {
                        self.showErrorAlert("Enter a valid file name")
                    }
                }
                else {
                    self.showErrorAlert("Enter a file name")
                }
            }
            else {
                self.showErrorAlert("Text field not found")
            }
        })
        let cancel = UIAlertAction(title: "Cancel", style: .cancel, handler: {
            UIAlertAction in
            // Does nothing
        })
        alert.addAction(ok)
        alert.addAction(cancel)
        self.present(alert, animated: true, completion: nil)
    }
    
    @IBAction func segmentedControlLMHValueChanged(_ sender: UISegmentedControl) {
        // Update currentGains
        let currentGains: Int = sender.selectedSegmentIndex
        switch currentGains {
        case 0:
            _ = audioController.update(currentGains: "Low")
            break
        case 1:
            _ = audioController.update(currentGains: "Medium")
            break
        case 2:
            _ = audioController.update(currentGains: "High")
            break
        default:
            updateLMH()
        }
        
        // Update all gains
        sliderGains = audioController.gains
        initGains = sliders.map({$0.value})
        
        // Update reset label
        labelResetThisGains.text = "Reset \(audioController.currentGains) Gains to Default"
    }
    
    @IBAction func sliderGainsValueChanged(_ sender: UISlider) {
        // Update gains, works for all gains because of computed property sliderGains
        _ = audioController.update(gains: sliderGains)
        // Now, gains are custom
        audioController.gainsFile = "Custom"
    }
    
    /// Shows an error alert if an error occurs when loading files.
    ///
    /// - Parameter message: The error message to show in the alert.
    func showErrorAlert(_ message: String) {
        let alert: UIAlertController = UIAlertController(title: "Save File Error",
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
    
    /// Updates the gains sliders when app becomes active.
    ///
    /// Selected to be called so that the imported file is shown.
    func appActiveAction(_ note: NSNotification) {
        if(self.isViewLoaded && self.view.window != nil) {
            // Show error from import if there is one pending
            if let importError = importedFileErrorString {
                if(importError != "") {
                    showImportErrorAlert(importError)
                    // Clear error string
                    importedFileErrorString = nil
                }
                else {
                    // If successful import, update init values
                    initGains = sliders.map({$0.value})
                }
            }
            sliderGains = audioController.gains
        }
    }
    
    /// Update the current gains as "Low", "Medium", or "High" based on the selection.
    func updateLMH() {
        // Set current gains to LMH
        let currentGains: String = audioController.currentGains
        switch currentGains {
        case "Low":
            segmentedControlLMH.selectedSegmentIndex = 0
            break
        case "Medium":
            segmentedControlLMH.selectedSegmentIndex = 1
            break
        case "High":
            segmentedControlLMH.selectedSegmentIndex = 2
            break
        default:
            segmentedControlLMH.selectedSegmentIndex = UISegmentedControlNoSegment
            
        }
    }
    
    // Selecting row which functions as a button
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        // Reset to default buttons
        if indexPath.section == 2 {
            if indexPath.row == 0 {
                // Reset these gains
                audioController.setDefault(all: false)
                sliderGains = audioController.gains
                tableView.deselectRow(at: indexPath, animated: true)
            }
            else if indexPath.row == 1 {
                // Reset all gains
                audioController.setDefault(all: true)
                audioController.gainsFile = "Default"
                sliderGains = audioController.gains
                tableView.deselectRow(at: indexPath, animated: true)
            }
        }
    }
}
