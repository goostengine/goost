#include "register_audio_types.h"

#include "goost/classes_enabled.gen.h"
#include "midi_player.h"

namespace goost {

void register_audio_types() {
	ClassDB::register_class<MidiPlayer>();
	ClassDB::register_class<MidiFile>();

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		Ref<ResourceImporterMidiFile> midifile;
		midifile.instance();
		ResourceFormatImporter::get_singleton()->add_importer(midifile);

	}
#endif
}

void unregister_audio_types() {

}

} // namespace goost
