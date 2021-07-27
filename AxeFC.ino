#include <Arduino.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_Client_ESP32.h>
#include "Fractal.h"
#include "OneButton.h"

void ReadCB(void *parameter);

typedef struct AppState_ {
  static const int ledPort = 1;
  bool isConnected;
  unsigned long lastUpdate;

  PRESET_NUMBER currentPreset = PRESET_NUMBER(BANK_0, 0);
  String currentPresetName;
  bool currentPresetNameOutdated;
  unsigned long currentPresetNameLastRequest;
  byte currentScene;
  String currentSceneName;
  bool currentSceneNameOutdated;
  unsigned long currentSceneNameLastRequest;
  bool tunerEnabled;

  static AppState_ empty() {
    auto s = AppState_();
    s.isConnected = false;
    s.lastUpdate = 0;
    s.currentPreset = PRESET_NUMBER(BANK_0, 0);
    s.currentPresetName = "";
    s.currentPresetNameOutdated = true;
    s.currentPresetNameLastRequest = 0;
    s.currentScene = 0;
    s.currentSceneName = "";
    s.currentSceneNameOutdated = true;
    s.currentSceneNameLastRequest = 0;
    s.tunerEnabled = false;
    return s;
  }
} AppState;

AppState state;

BLEMIDI_CREATE_INSTANCE("AxeFxBLE", MIDI);
OneButton previousButton(2, false);
OneButton selectButton(15, false);
OneButton nextButton(4, false);

void setup()
{
  state = AppState::empty();
  state.lastUpdate = millis();
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  previousButton.attachClick(previousButtonClick);
  selectButton.attachClick(selectButtonClick);
  nextButton.attachClick(nextButtonClick);
  previousButton.attachLongPressStart(previousButtonLongPress);
  selectButton.attachLongPressStart(selectButtonLongPress);
  nextButton.attachLongPressStart(nextButtonLongPress);

  BLEMIDI.setHandleConnected(onConnected);
  BLEMIDI.setHandleDisconnected(onDisconnected);
  MIDI.setHandleSystemExclusive(onMidiSysEx);
  MIDI.setHandleProgramChange(onProgramChange);
  MIDI.setHandleControlChange(onControlChange);
  xTaskCreatePinnedToCore(readCB, "MIDI-READ", 3000, NULL, 1, NULL, 1);

  pinMode(state.ledPort, OUTPUT);
  digitalWrite(state.ledPort, state.isConnected ? HIGH : LOW);
}

void loop()
{
  if (state.isConnected && (millis() - state.lastUpdate) > 1000)
  {
    state.lastUpdate = millis();
    requestCurrentPresetNameIfNeeded();
    // requestCurrentSceneNameIfNeeded();
  }
  previousButton.tick();
  selectButton.tick();
  nextButton.tick();
  vTaskDelay(10);
}

void readCB(void *parameter)
{
  for (;;)
  {
    MIDI.read();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  vTaskDelay(1);
}

void onConnected() {
  state.isConnected = true;
  digitalWrite(state.ledPort, state.isConnected ? HIGH : LOW);
}

void onDisconnected() {
  state.isConnected = false;
  digitalWrite(state.ledPort, state.isConnected ? HIGH : LOW);
}

void onProgramChange(byte channel, byte program) {
  Serial.print(F("PC: ("));
  Serial.print(program);
  Serial.print(F(") "));
  if (state.currentPreset.program == program) {
    return;
  }
  auto currentBank = state.currentPreset.bank;

  state.currentPreset = PRESET_NUMBER(currentBank, program);
  state.currentPresetName = "";
  state.currentPresetNameOutdated = true;
  state.currentScene = 0;
  state.currentSceneName = "";
  state.currentSceneNameOutdated = true;
}

void onControlChange(byte channel, byte control, byte value) {
  if (control == 0) {
    onBankChange(value);
  }
  Serial.print(F("CC: ("));
  Serial.print(control);
  Serial.print(F(", "));
  Serial.print(value);
  Serial.print(F(") "));
}

void onBankChange(byte bank) {
  if (state.currentPreset.bank == bank) {
    return;
  }
  auto currentProgram = state.currentPreset.program;

  state.currentPreset = PRESET_NUMBER(BANK(bank), currentProgram);
  state.currentPresetName = "";
  state.currentPresetNameOutdated = true;
  state.currentScene = 0;
  state.currentSceneName = "";
  state.currentSceneNameOutdated = true;
}

void onMidiSysEx(byte* data, unsigned length) {
  Serial.print(F("SYSEX: ("));
  Serial.print(length);
  Serial.print(F(" bytes) "));
  for (uint16_t i = 0; i < length; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" - ");
  for (uint16_t i = 0; i < length; i++)
  {
    Serial.print((char)data[i]);
    Serial.print(" ");
  }
  Serial.println();

  checkForPresetName(data, length);
  checkForSceneName(data, length);
}

void checkForPresetName(byte* data, unsigned length) {
  if (length < 8) {
    return;
  }
  if (data[0] != 0xF0 || data[1] != 0x00 || data[2] != 0x01 || data[3] != 0x74 || data[4] != 0x10 || data[5] != 0x0D) {
    return;
  }

  state.currentPreset = PRESET_NUMBER(BANK(data[7]), data[6]);
  state.currentPresetNameOutdated = false;
  Serial.print("-> Current preset is ");
  Serial.print(state.currentPreset.position());
  Serial.print(" or bank ");
  Serial.print(state.currentPreset.bank);
  Serial.print(" program ");
  Serial.print(state.currentPreset.program);
  Serial.println();
}

void checkForSceneName(byte* data, unsigned length) {
  if (length < 7) {
    return;
  }
  if (data[0] != 0xF0 || data[1] != 0x00 || data[2] != 0x01 || data[3] != 0x74 || data[4] != 0x10 || data[5] != 0x0D) {
    return;
  }
  state.currentScene = data[6];
  state.currentSceneNameOutdated = false;
  Serial.print("-> Current scene is ");
  Serial.print(state.currentScene);
  Serial.println();
}

void requestCurrentSceneNameIfNeeded() {
  if (!state.currentSceneNameOutdated) {
    return;
  }
  if (!state.isConnected) {
    return;
  }
  if (
    state.currentSceneNameLastRequest != 0 &&
    millis() > state.currentSceneNameLastRequest &&
    state.currentSceneNameLastRequest + 10000 > millis()
  ) {
    return;
  }

  auto sysex = SceneNameQuerySysEx(CURRENT_SCENE);
  byte * data;
  unsigned len = sysex.serialize(&data);

  Serial.println("Requesting scene name");
  state.currentSceneNameLastRequest = millis();
  MIDI.sendSysEx(len, data, true);
}

void requestCurrentPresetNameIfNeeded() {
  if (!state.currentPresetNameOutdated) {
    return;
  }
  if (!state.isConnected) {
    return;
  }
  if (
    state.currentPresetNameLastRequest != 0 &&
    millis() > state.currentPresetNameLastRequest &&
    state.currentPresetNameLastRequest + 10000 > millis()
  ) {
    return;
  }

  auto sysex = PresetNameQuerySysEx();
  byte * data;
  Serial.println("Will serialize");
  unsigned len = sysex.serialize(&data);

  Serial.print("Requesting preset name (");
  Serial.print(len);
  Serial.print(" bytes):");
  for (int i = 0; i < len; i++) {
    Serial.print("0x");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  state.currentPresetNameLastRequest = millis();
  MIDI.sendSysEx(len, data, true);
}

byte current = 29;
void previousButtonClick() {
  Serial.println("previousButtonClick");
  MIDI.sendControlChange(33, 0, 1);
}
void selectButtonClick() {
  Serial.println("selectButtonClick");
}
void nextButtonClick() {
  Serial.println("nextButtonClick");
  MIDI.sendControlChange(35, 0, 1);   
}

void previousButtonLongPress() {
  Serial.println("previousButtonLongPress");
  MIDI.sendControlChange(30, 0, 1);
//  MIDI.sendControlChange(0, 3, 1);
//  MIDI.sendProgramChange(--current, 1);
}
void selectButtonLongPress() {
  Serial.println("selectButtonLongPress");
  state.tunerEnabled = !state.tunerEnabled;
  if (state.tunerEnabled) {
    byte sysex[] = { 0xF0, 0x00, 0x01, 0x74, 0x10, 0x11, 0x01, 0x85 & 0x7F, 0xF7 };
    MIDI.sendSysEx(9, sysex, true);
  } else {
    byte sysex[] = { 0xF0, 0x00, 0x01, 0x74, 0x10, 0x11, 0x00, 0x84 & 0x7F, 0xF7 };
    MIDI.sendSysEx(9, sysex, true);
  }
}
void nextButtonLongPress() {
  Serial.println("nextButtonLongPress");
  MIDI.sendControlChange(31, 0, 1);
  // auto sysex = SceneSysEx(SCENE_1);
  // MIDI.sendControlChange(0, 3, 1);
  // MIDI.sendProgramChange(29, 1);
  // auto presetNumber = PRESET_NUMBER();
  // presetNumber.bank = BANK_3;
  // presetNumber.program = 300;
  // auto sysex = PresetNameQuerySysEx(presetNumber);
//  MIDI.sendControlChange(0, 3, 1);
//  MIDI.sendProgramChange(++current, 1);
}
