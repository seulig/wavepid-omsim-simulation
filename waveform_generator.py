from icecube import icetray, dataio, WaveCalibrator, wavedeform, wavereform
import numpy as np

class OMSimReader(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("Filename", "Path to the OMSim output file", "")
        self.AddOutBox("OutBox")

    def Configure(self):
        self.filename = self.GetParameter("Filename")
        self.file = open(self.filename, "r")
        self.file.readline() #skip header

    def Process(self):
        frame = icetray.I3Frame(icetray.I3Frame.DAQ)
        line = self.file.readline()
        if not line:
            self.file.close()
            self.PushFrame(frame)
            return False
        
        data = line.split()
        if len(data) < 7:
            return True
        
        event_id = int(data[0])
        pmt_nr = int(data[1])
        hit_time = float(data[2])
        pe = float(data[3])
        transit_time = float(data[4])
        detection_probability = float(data[5])
        wavelength = float(data[6])

        omkey = icetray.OMKey(0, pmt_nr)
        pulses = icetray.I3RecoPulseSeries()
        pulse = icetray.I3RecoPulse()
        pulse.time = hit_time
        pulse.charge = pe
        pulses.append(pulse)

        pulse_map = icetray.I3RecoPulseSeriesMap()
        pulse_map[omkey] = pulses
        frame["OMSimPulses"] = pulse_map

        frame["OMSimWavelength"] = wavelength
        self.PushFrame(frame)
        return True

tray = icetray.I3Tray()

tray.AddModule("OMSimReader", "omsim_reader",
    Filename="output_test_hits.dat" # Replace with your OMSim output file
    )

tray.AddModule("I3Wavedeform", "deform",
    InputPulses="OMSimPulses",
    OutputPulses="WavedeformPulses",
    PulseType="SPE",
    )

tray.AddModule("I3Wavereform", "wavereform_atwd",
    Waveforms="CalibratedWaveforms",
    Pulses="WavedeformPulses",
    Chi="Chi_HLCPulses_CalibratedATWD",
    ChiThreshold=1e4, # flag ~ 0.2% of waveforms
    Flag="Borked_ATWDs",
    )

tray.AddModule("I3Writer", "writer",
    Filename="output_waveform.i3.gz"
    )

tray.Execute()