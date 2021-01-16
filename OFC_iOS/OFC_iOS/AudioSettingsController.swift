//
//  AudioSettingsController.swift
//  SPP Equalizer UI 1
//
//  Created by Akshay Chitale on 6/25/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import Foundation

/// Globally availible audio settings, set in `AppDelegate.swift` on app launch and for use in all view controllers.
var audioController: AudioSettingsController!

/// A controller for starting and stopping the audio, as well as changing the audio settings.
///
/// This class serves as an interface for Swift code to interact with the underlying Objective-C and C audio code.
class AudioSettingsController {
    // Later have private var for IosAudioController, for now is global
    
    
    
    
    
    /// Initializes the `AudioSettingsController` with "Low", "Medium", and "High" gains and default settings.
    init() {
        iosAudio = IosAudioController() // Is global for now, make local later
        
    }
    /// Initializes the `AudioSettingsController` with an array of current gains.
    ///
    
    deinit {
        // Deallocate memory
        iosAudio.destroySettings()
    }
    

    
    
    /// Starts the audio.
    ///
    /// This function is safe to use whether or not the audio is currently stopped.
    func start() {
        if !playAudio {
            iosAudio.start()
            iosAudio.settings.pointee.playAudio = 1
        }
    }
    
    /// Stops the audio.
    ///
    /// This function is safe to use whether or not the audio is currently playing.
    func stop() {
        if playAudio {
            iosAudio.stop()
            iosAudio.settings.pointee.playAudio = 0
        }
    }
    
  
    
    //if(self.autoGains) {}
    /// Whether the Reading otions are being selected.
    var LoadingButt: Bool {
        return iosAudio.settings.pointee.LoadingButt != 0 ? true : false
    }
    
    /// Whether the saving otions are being selected.
    var SavingButt: Bool {
        return iosAudio.settings.pointee.SavingButt != 0 ? true : false
    }
    
    /// Whether the saving features option are being selected.
    var SavingFeatButt: Bool {
        return iosAudio.settings.pointee.SavingFeatButt != 0 ? true : false
    }
    
    /// The user will define the noise type
    //var UserDefinedNoiseType: String {
    //    return String(describing: iosAudio.settings.pointee.UserDefinedNoiseType)
    //}
    

    /// The interval to wait before updating the screen for clusterong resutls, in seconds.
    var DisplayUpdateRate: Float {
        return iosAudio.settings.pointee.DisplayUpdateRate
    }

    /// The interval that clustering resutls are computed, in seconds.
    var ClassifficUpdateRate: Float {
        return iosAudio.settings.pointee.ClassifficUpdateRate
    }

    
    /// The current sampling frequency.
    var fs: Int {
        return Int(iosAudio.settings.pointee.fs)
    }
    
    /// max number of detected clusters
    var ClusterSize: Int {
        return Int(iosAudio.settings.pointee.ClusterSize)
    }
    
    /// The current Chunk size (number of frames in Chunk).
    var sizeL: Int {
        return Int(iosAudio.settings.pointee.sizeL)
    }
    
    /// The current sigma .
    var sigma: Double {
        return Double(iosAudio.settings.pointee.sigma)
    }
    
    /// The current fraction Rejection .
    var fractionRej: Double {
        return Double(iosAudio.settings.pointee.fractionRej)
    }
    
    /// The current frame size, a.k.a. the window size.
    var frameSize: Int {
        return Int(iosAudio.settings.pointee.frameSize)
    }
    
    /// The current step size, a.k.a. the overlap size.
    var stepSize: Int {
        return Int(iosAudio.settings.pointee.stepSize)
    }
    
    /// The current microphone status. Is using audio input from the microphone if true, or from a file if false.
    var micStatus: Bool {
        return iosAudio.settings.pointee.micStatus != 0 ? true : false
    }
    
    /// Whether audio is currently playing.
    var playAudio: Bool {
        return iosAudio.settings.pointee.playAudio != 0 ? true : false
    }
    
    
    /// The audio level, in dB SPL.
    var dbpower: Float {
        return iosAudio.settings.pointee.dbpower
    }
    
    
    /// The interval to wait before updating the audio level, in seconds.
    var dbUpdateInterval: Float {
        return iosAudio.settings.pointee.dbUpdateInterval
    }
    
  
    
    
    /// Updates the sampling frequency.
    ///
    
    /// Whether the Reading otions are being selected.
    func update (LoadingButt: Bool) {
        iosAudio.settings.pointee.LoadingButt = Int32(LoadingButt ? 1 : 0)
    }
    
    /// Whether the saving otions are being selected.
    func update (SavingButt: Bool) {
        iosAudio.settings.pointee.SavingButt = Int32(SavingButt ? 1 : 0)
    }
    
    /// Whether the saving features option are being selected.
    func update (SavingFeatButt: Bool) {
        iosAudio.settings.pointee.SavingFeatButt = Int32(SavingFeatButt ? 1 : 0)
    }
    
    /// Updates the noise type given by the user.
    /// - Parameter fileName: The name of the noise type,
    //func update(UserDefinedNoiseType: String) {
        // Safe to just point, since inner code never mutates C string
    //    iosAudio.settings.pointee.UserDefinedNoiseType = UnsafePointer<Int8>(UserDefinedNoiseType)
    //}
    
    /// Updates The interval to wait before updating the screen for clusterong resutls, in seconds.
    ///
    /// - Parameter dbUpdateInterval: The new interval to wait, in seconds.
    func update(DisplayUpdateRate: Float) {
        iosAudio.settings.pointee.DisplayUpdateRate = DisplayUpdateRate
    }
 
    /// Updates the interval that clustering resutls are computed, in seconds.
    ///
    /// - Parameter dbUpdateInterval: The new interval to wait, in seconds.
    func update(ClassifficUpdateRate: Float) {
        iosAudio.settings.pointee.ClassifficUpdateRate = ClassifficUpdateRate
    }
    

    /// - Parameter fs: The new sampling frequency, in Hertz.
    func update(fs: Int) {
        iosAudio.settings.pointee.fs = Int32(fs)
    }
    
    /// update max number of detected clusters
    func update (ClusterSize: Int) {
        iosAudio.settings.pointee.ClusterSize = Int32(ClusterSize)
    }
    
    /// The current Chunk size (number of frames in Chunk).
    func update (sizeL: Int) {
        iosAudio.settings.pointee.sizeL = Int32(sizeL)
    }
    
    /// The current sigma .
    func update (sigma: Double) {
        iosAudio.settings.pointee.sigma = double_t(sigma)
    }
    
    /// The current fraction Rejection .
    func update (fractionRej: Double) {
        iosAudio.settings.pointee.fractionRej = double_t(fractionRej)
    }
    /// Updates the frame size, a.k.a. the window size.
    ///
    /// - Parameter frameSize: The new frame size, in milliseconds.
    func update(frameSize: Float) {
        iosAudio.settings.pointee.frameSize = Int32(frameSize * Float(fs) / 1000.0)
    }
    
    /// Updates the step size, a.k.a. the overlap size.
    ///
    /// - Parameter stepSize: The new step size, in milliseconds.
    func update(stepSize: Float) {
        iosAudio.settings.pointee.stepSize = Int32(stepSize * Float(fs) / 1000.0)
    }
    
    /// Updates the microphone status.
    ///
    /// - Parameter micStatus: If true, use microphone audio input. If false, use audio input from file.
    func update(micStatus: Bool) {
        iosAudio.settings.pointee.micStatus = Int32(micStatus ? 1 : 0)
    }
    
    

    
    /// Updates the interval to wait before updating the audio level.
    ///
    /// - Parameter dbUpdateInterval: The new interval to wait, in seconds.
    func update(dbUpdateInterval: Float) {
        iosAudio.settings.pointee.dbUpdateInterval = dbUpdateInterval
    }
    
    
}
