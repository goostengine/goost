extends "res://addons/gut/test.gd"

func test_midi_file():
    var midi = MidiFile.new()
    midi.data = PoolByteArray([20, 180, 173])
    midi.data = PoolByteArray([100])
    assert_true(midi.data.size() > 0)
