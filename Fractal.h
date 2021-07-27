// BASICS

typedef enum BANK_ : byte {
  BANK_0 = 0,
  BANK_1,
  BANK_2,
  BANK_3,
  BANK_4,
  BANK_5,
  BANK_6,
  BANK_7
} BANK;

typedef struct PRESET_NUMBER_ {
  BANK bank;
  byte program;

  PRESET_NUMBER_(BANK bank, byte program) : bank(bank), program(program) { }

  unsigned int position() {
    return program + int(bank) * 128;
  }

} PRESET_NUMBER;

typedef enum EFFECT_ID_ {
  ID_CONTROL = 2,
  ID_TUNER = 35,
  ID_IRCAPTURE,
  ID_INPUT1,
  ID_INPUT2,
  ID_INPUT3,
  ID_INPUT4,
  ID_INPUT5,
  ID_OUTPUT1,
  ID_OUTPUT2,
  ID_OUTPUT3,
  ID_OUTPUT4,
  ID_COMP1,
  ID_COMP2,
  ID_COMP3,
  ID_COMP4,
  ID_GRAPHEQ1,
  ID_GRAPHEQ2,
  ID_GRAPHEQ3,
  ID_GRAPHEQ4,
  ID_PARAEQ1,
  ID_PARAEQ2,
  ID_PARAEQ3,
  ID_PARAEQ4,
  ID_DISTORT1,
  ID_DISTORT2,
  ID_DISTORT3,
  ID_DISTORT4,
  ID_CAB1,
  ID_CAB2,
  ID_CAB3,
  ID_CAB4,
  ID_REVERB1,
  ID_REVERB2,
  ID_REVERB3,
  ID_REVERB4,
  ID_DELAY1,
  ID_DELAY2,
  ID_DELAY3,
  ID_DELAY4,
  ID_MULTITAP1,
  ID_MULTITAP2,
  ID_MULTITAP3,
  ID_MULTITAP4,
  ID_CHORUS1,
  ID_CHORUS2,
  ID_CHORUS3,
  ID_CHORUS4,
  ID_FLANGER1,
  ID_FLANGER2,
  ID_FLANGER3,
  ID_FLANGER4,
  ID_ROTARY1,
  ID_ROTARY2,
  ID_ROTARY3,
  ID_ROTARY4,
  ID_PHASER1,
  ID_PHASER2,
  ID_PHASER3,
  ID_PHASER4,
  ID_WAH1,
  ID_WAH2,
  ID_WAH3,
  ID_WAH4,
  ID_FORMANT1,
  ID_FORMANT2,
  ID_FORMANT3,
  ID_FORMANT4,
  ID_VOLUME1,
  ID_VOLUME2,
  ID_VOLUME3,
  ID_VOLUME4,
  // USB Input
  ID_TREMOLO1,
  ID_TREMOLO2,
  ID_TREMOLO3,
  ID_TREMOLO4,
  ID_PITCH1,
  ID_PITCH2,
  ID_PITCH3,
  ID_PITCH4,
  ID_FILTER1,
  ID_FILTER2,
  ID_FILTER3,
  ID_FILTER4,
  ID_FUZZ1,
  ID_FUZZ2,
  ID_FUZZ3,
  ID_FUZZ4,
  ID_ENHANCER1,
  ID_ENHANCER2,
  ID_ENHANCER3,
  ID_ENHANCER4,
  ID_MIXER1,
  ID_MIXER2,
  ID_MIXER3,
  ID_MIXER4,
  ID_SYNTH1,
  ID_SYNTH2,
  ID_SYNTH3,
  ID_SYNTH4,
  ID_VOCODER1,
  ID_VOCODER2,
  ID_VOCODER3,
  ID_VOCODER4,
  ID_MEGATAP1,
  ID_MEGATAP2,
  ID_MEGATAP3,
  ID_MEGATAP4,
  ID_CROSSOVER1,
  ID_CROSSOVER2,
  ID_CROSSOVER3,
  ID_CROSSOVER4,
  ID_GATE1,
  ID_GATE2,
  ID_GATE3,
  ID_GATE4,
  ID_RINGMOD1,
  ID_RINGMOD2,
  ID_RINGMOD3,
  ID_RINGMOD4,
  ID_MULTICOMP1,
  ID_MULTICOMP2,
  ID_MULTICOMP3,
  ID_MULTICOMP4,
  ID_TENTAP1,
  ID_TENTAP2,
  ID_TENTAP3,
  ID_TENTAP4,
  ID_RESONATOR1,
  ID_RESONATOR2,
  ID_RESONATOR3,
  ID_RESONATOR4,
  ID_LOOPER1,
  ID_LOOPER2,
  ID_LOOPER3,
  ID_LOOPER4,
  ID_TONEMATCH1,
  ID_TONEMATCH2,
  ID_TONEMATCH3,
  ID_TONEMATCH4,
  ID_RTA1,
  ID_RTA2,
  ID_RTA3,
  ID_RTA4,
  ID_PLEX1,
  ID_PLEX2,
  ID_PLEX3,
  ID_PLEX4,
  ID_FBSEND1,
  ID_FBSEND2,
  ID_FBSEND3,
  ID_FBSEND4,
  ID_FBRETURN1,
  ID_FBRETURN2,
  ID_FBRETURN3,
  ID_FBRETURN4,
  ID_MIDIBLOCK,
  ID_MULTIPLEXER1,
  ID_MULTIPLEXER2,
  ID_MULTIPLEXER3,
  ID_MULTIPLEXER4,
  ID_IRPLAYER1,
  ID_IRPLAYER2,
  ID_IRPLAYER3,
  ID_IRPLAYER4,
  ID_FOOTCONTROLLER,
  ID_PRESET_FC,
} EFFECT_ID;

typedef enum OP_CODE_ : byte {
  SET_GET_EFFECT_BYPASS = 0x0A,
  SET_GET_EFFECT_CHANNEL = 0x0B,
  SET_GET_SCENE = 0x0C,
  GET_PRESET_NAME = 0x0D,
  GET_SCENE_NAME = 0x0E
} OP_CODE;

class FractalSysEx {
  public:
    int serialize(byte *stream[]) {
      int header_size = sizeof(header);
      int final_length = header_size + payload_length + 3; /* opcode, checksum, bye */
      byte return_value[final_length];
      // byte checksum = 0;
      int position = 0;

      // Header (0..<header_size)
      for (byte header_byte : header) {
        return_value[position++] = header_byte;
        // checksum ^= header_byte;
      }

      // Opcode (header_size)
      return_value[position++] = opcode;
      // checksum ^= opcode;

      // Payload (header_size + 1 ..< header_size + 1 + payload_size)
      for (int i = 0; i < payload_length; i++) {
        byte payload_byte = payload[i];
        return_value[position++] = payload_byte;
        // checksum ^= payload_byte;
      }

      // Checksum (header_size + 1 + payload_size)
      return_value[position++] = calculateChecksum(); // (checksum & 0x7F);

      // Bye (header_size + 1 + payload_size + 1)
      return_value[position++] = bye;

      *stream = return_value;

      Serial.print("Serialized command (");
      Serial.print(position);
      Serial.print(" bytes ): ");
      for (int i = 0; i < final_length; i++) {
        Serial.print("0x");
        Serial.print(return_value[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      return position;
    }

  protected:
    FractalSysEx(const OP_CODE opcode, byte *payload, const int payload_length) : opcode(opcode), payload(payload), payload_length(payload_length) { }
    const byte header[5] = {0xF0, 0x00, 0x01, 0x74, 0x10};
    const OP_CODE opcode;
    byte *payload;
    const int payload_length;
    const byte bye = 0xF7;

    static int midiToInt16(const byte *bytes) {
      return bytes[0] + bytes[1] * 0x7F;
    }

    static void uint16ToMidi(const unsigned int number, byte *byte1, byte *byte2) {
      *byte1 = number % 0x7F;
      *byte2 = number / 0x7F;
    }

    byte calculateChecksum() {
      byte checksum = 0;

      for (int i = 0; i < sizeof(header); i++) {
        byte header_byte = header[i];
        checksum ^= header_byte;
      }
      checksum ^= byte(opcode);
      for (int i = 0; i < payload_length; i++) {
        byte payload_byte = payload[i];
        checksum ^= payload_byte;
      }

      return checksum & 0x7F;
    }

    static void presetToMidi(PRESET_NUMBER preset_number, byte *byte1, byte *byte2) {
      unsigned int value = preset_number.position();
      FractalSysEx::uint16ToMidi(value, byte1, byte2);
    }

    bool validate_header(byte *message, unsigned int length) {
      if (length < sizeof(header)) {
        return false;
      }
      for (int i = 0; i < length; i++) {
        if (message[i] != header[i]) {
          return false;
        }
      }
      return true;
    }

    ~FractalSysEx() {
      free(payload);
    }
};

// Effect Bypass

typedef enum BYPASS_COMMAND_ : byte {
  SET_ENGAGED = 0,
  SET_BYPASSED,
  GET_BYPASS_STATUS = 0x7F
} BYPASS_COMMAND;

class EffectBypassSysEx: public FractalSysEx {
  public:
    EffectBypassSysEx(const EFFECT_ID effect_id, const BYPASS_COMMAND bypass_command)
      : EffectBypassSysEx(EffectBypassSysEx::create_payload(effect_id, bypass_command), payload_length) { }

  private:
    static const int payload_length = 3;
    EffectBypassSysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::SET_GET_EFFECT_BYPASS, payload, payload_length) { }

    static byte *create_payload(const EFFECT_ID effect_id, const BYPASS_COMMAND bypass_command) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      byte *effect_id_bytes;
      FractalSysEx::uint16ToMidi(int(effect_id), &data[0], &data[1]);
      data[2] = bypass_command;
      return data;
    }
};

// Effect Channel

typedef enum CHANNEL_COMMAND_ : byte {
  SET_CHANNEL_A = 0,
  SET_CHANNEL_B,
  SET_CHANNEL_C,
  SET_CHANNEL_D,
  GET_CHANNEL_STATUS = 0x7F
} CHANNEL_COMMAND;

class EffectChannelSysEx: public FractalSysEx {
  public:
    EffectChannelSysEx(const EFFECT_ID effect_id, const CHANNEL_COMMAND channel_command)
      : EffectChannelSysEx(EffectChannelSysEx::create_payload(effect_id, channel_command), payload_length) { }

  private:
    static const int payload_length = 3;
    EffectChannelSysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::SET_GET_EFFECT_CHANNEL, payload, payload_length) { }

    static byte *create_payload(const EFFECT_ID effect_id, const CHANNEL_COMMAND channel_command) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      FractalSysEx::uint16ToMidi(int(effect_id), &data[0], &data[1]);
      data[2] = channel_command;
      return data;
    }
};

// Scene

typedef enum SCENE_COMMAND_ : byte {
  SCENE_1 = 0,
  SCENE_2,
  SCENE_3,
  SCENE_4,
  SCENE_5,
  SCENE_6,
  SCENE_7,
  SCENE_8,
  CURRENT_SCENE = 0x7F
} SCENE_COMMAND;

class SceneSysEx: public FractalSysEx {
  public:
    SceneSysEx(const SCENE_COMMAND scene_command)
      : SceneSysEx(SceneSysEx::create_payload(scene_command), payload_length) { }

  private:
    static const int payload_length = 1;
    SceneSysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::SET_GET_SCENE, payload, payload_length) { }

    static byte *create_payload(const SCENE_COMMAND scene_command) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      data[0] = { scene_command };
      return data;
    }
};

// Preset Name

class PresetNameQuerySysEx: public FractalSysEx {
  public:
    PresetNameQuerySysEx(const PRESET_NUMBER preset_number)
      : PresetNameQuerySysEx(PresetNameQuerySysEx::create_payload(preset_number), payload_length) { }

    PresetNameQuerySysEx()
      : PresetNameQuerySysEx(PresetNameQuerySysEx::create_payload_for_get_current(), payload_length) { }

  private:
    static const int payload_length = 2;
    PresetNameQuerySysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::GET_PRESET_NAME, payload, payload_length) { }

    static byte *create_payload_for_get_current() {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      data[0] = 0x7F;
      data[1] = 0x7F;
      return data;
    }

    static byte *create_payload(const PRESET_NUMBER preset_number) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      byte first;
      byte second;
      FractalSysEx::presetToMidi(preset_number, &first, &second);
      data[0] = first;
      data[1] = second;
      return data;
    }
};

class PresetNameResponseSysEx: public FractalSysEx {
  public:
    PresetNameResponseSysEx(const String preset_name, PRESET_NUMBER preset_number)
      : PresetNameResponseSysEx(PresetNameResponseSysEx::create_payload(preset_name, preset_number), payload_length) { }

    unsigned int preset_number() {
      byte preset_number_bytes[] = { payload[0], payload[1] };
      return midiToInt16(preset_number_bytes);
    }

    String preset_name() {
      String preset_name = "";
      for (int i = 0; i < 32; i++) {
        char ch = payload[i + 2];
        if (ch == 0x00) {
          continue;
        }
        preset_name.concat(ch);
      }
      return preset_name;
    }

    static bool validate(byte *message, unsigned int length) {
      unsigned int header_size = sizeof(header);
      if (length < header_size + payload_length + 2) {
        return false;
      }
      // if (!validate_header(message, length)) { return false; }

      // byte checksum_byte = checksum(header, opcode, byte *message, length);
      return true;
    }

  private:
    static const int payload_length = 34;
    PresetNameResponseSysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::GET_PRESET_NAME, payload, payload_length) { }

    static byte *create_payload(const String preset_name, PRESET_NUMBER preset_number) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      byte first;
      byte second;
      FractalSysEx::presetToMidi(preset_number, &first, &second);
      data[0] = first;
      data[1] = second;
      for (int i = 0; i < 32; i++) {
        if (i < preset_name.length()) {
          data[i + 2] = preset_name.charAt(i);
        } else {
          data[i + 2] = 0;
        }
      }
      return data;
    }
};

// Scene Name

class SceneNameQuerySysEx: public FractalSysEx {
  public:
    SceneNameQuerySysEx(const SCENE_COMMAND scene_command)
      : SceneNameQuerySysEx(SceneNameQuerySysEx::create_payload(scene_command), payload_length) { }

  private:
    static const int payload_length = 1;
    SceneNameQuerySysEx(byte *payload, const int payload_length)
      : FractalSysEx(OP_CODE::GET_SCENE_NAME, payload, payload_length) { }

    static byte *create_payload(const SCENE_COMMAND scene_command) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      data[0] = byte(scene_command);
      return data;
    }
};

class SceneNameResponseSysEx: public FractalSysEx {
  public:
    SceneNameResponseSysEx(const String scene_name, const SCENE_COMMAND scene_number)
      : SceneNameResponseSysEx(SceneNameResponseSysEx::create_payload(scene_name, scene_number), payload_length) { }

    SCENE_COMMAND scene_number() {
      return SCENE_COMMAND(payload[0]);
    }

    String scene_name() {
      String scene_name = "";
      for (int i = 0; i < 32; i++) {
        char ch = payload[i + 1];
        if (ch == 0x00) {
          continue;
        }
        scene_name.concat(ch);
      }
      return scene_name;
    }

  private:
    static const int payload_length = 33;
    SceneNameResponseSysEx(byte *payload, int payload_length)
      : FractalSysEx(OP_CODE::GET_SCENE_NAME, payload, payload_length) { }

    static byte *create_payload(const String scene_name, const SCENE_COMMAND scene_number) {
      byte *data = (byte *) malloc(payload_length * sizeof(byte));
      data[0] = byte(scene_number);
      for (int i = 0; i < 32; i++) {
        if (i < scene_name.length()) {
          data[i + 1] = scene_name.charAt(i);
        } else {
          data[i + 1] = 0;
        }
      }
      return data;
    }
};
