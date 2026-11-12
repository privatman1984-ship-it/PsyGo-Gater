#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr auto loopParameter = "loopBars";
constexpr auto depthParameter = "depth";
constexpr auto wetParameter = "wet";
constexpr auto attackParameter = "attackMs";
constexpr auto releaseParameter = "releaseMs";
constexpr auto swingParameter = "swing";
constexpr double fallbackBpm = 120.0;
constexpr double beatsPerBar = 4.0;
}

GoatranceGaterAudioProcessor::GoatranceGaterAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout GoatranceGaterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        loopParameter, "Loop", juce::StringArray { "1 bar", "2 bars", "4 bars" }, 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        depthParameter, "Depth", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.85f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        wetParameter, "Wet", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        attackParameter, "Attack", juce::NormalisableRange<float> (0.1f, 100.0f, 0.1f), 2.0f, "ms"));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        releaseParameter, "Release", juce::NormalisableRange<float> (0.1f, 250.0f, 0.1f), 18.0f, "ms"));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        swingParameter, "Swing", juce::NormalisableRange<float> (0.0f, 0.45f, 0.001f), 0.12f));

    for (int step = 0; step < 16; ++step)
        layout.add (std::make_unique<juce::AudioParameterBool> (
            "step" + juce::String (step + 1), "Step " + juce::String (step + 1), true));

    return layout;
}

const juce::String GoatranceGaterAudioProcessor::getName() const { return JucePlugin_Name; }
bool GoatranceGaterAudioProcessor::acceptsMidi() const { return false; }
bool GoatranceGaterAudioProcessor::producesMidi() const { return false; }
bool GoatranceGaterAudioProcessor::isMidiEffect() const { return false; }
double GoatranceGaterAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int GoatranceGaterAudioProcessor::getNumPrograms() { return 1; }
int GoatranceGaterAudioProcessor::getCurrentProgram() { return 0; }
void GoatranceGaterAudioProcessor::setCurrentProgram (int) {}
const juce::String GoatranceGaterAudioProcessor::getProgramName (int) { return {}; }
void GoatranceGaterAudioProcessor::changeProgramName (int, const juce::String&) {}

void GoatranceGaterAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    freeRunningPpq = 0.0;
    envelope = 1.0f;
}

void GoatranceGaterAudioProcessor::releaseResources() {}

bool GoatranceGaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& input = layouts.getMainInputChannelSet();
    const auto& output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void GoatranceGaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused (midi);
    juce::ScopedNoDenormals noDenormals;

    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = getTotalNumOutputChannels();
    const auto depth = parameters.getRawParameterValue (depthParameter)->load();
    const auto wet = parameters.getRawParameterValue (wetParameter)->load();
    const auto attackMs = parameters.getRawParameterValue (attackParameter)->load();
    const auto releaseMs = parameters.getRawParameterValue (releaseParameter)->load();
    const auto swing = parameters.getRawParameterValue (swingParameter)->load();
    const auto loopChoice = static_cast<int> (parameters.getRawParameterValue (loopParameter)->load());
    const auto loopBeats = beatsPerBar * static_cast<double> (loopChoice + 1);

    auto bpm = fallbackBpm;
    auto startPpq = freeRunningPpq;
    if (auto* playHead = getPlayHead())
    {
        const auto position = playHead->getPosition();
        if (position.hasValue())
        {
            const auto hostBpm = position->getBpm();
            const auto hostPpq = position->getPpqPosition();
            if (hostBpm.hasValue() && hostPpq.hasValue() && *hostBpm > 0.0)
            {
                bpm = *hostBpm;
                startPpq = *hostPpq;
            }
        }
    }

    const auto phaseIncrement = bpm / (60.0 * currentSampleRate);
    const auto attackCoefficient = static_cast<float> (1.0 - std::exp (-1.0 / (currentSampleRate * attackMs * 0.001)));
    const auto releaseCoefficient = static_cast<float> (1.0 - std::exp (-1.0 / (currentSampleRate * releaseMs * 0.001)));
    auto** channels = buffer.getArrayOfWritePointers();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto ppq = startPpq + static_cast<double> (sample) * phaseIncrement;
        auto cycle = std::fmod (ppq, loopBeats);
        if (cycle < 0.0)
            cycle += loopBeats;

        auto stepPosition = cycle / loopBeats * 16.0;
        auto step = static_cast<int> (stepPosition);
        const auto fraction = stepPosition - static_cast<double> (step);
        if ((step & 1) != 0 && fraction < swing)
            step = (step + 15) & 15;

        const auto stepIsOn = parameters.getRawParameterValue ("step" + juce::String (step + 1))->load() >= 0.5f;
        const auto target = stepIsOn ? 1.0f : 0.0f;
        const auto coefficient = target > envelope ? attackCoefficient : releaseCoefficient;
        envelope += coefficient * (target - envelope);
        const auto gatedGain = 1.0f - depth * (1.0f - envelope);
        const auto gain = 1.0f + wet * (gatedGain - 1.0f);

        for (int channel = 0; channel < numChannels; ++channel)
            channels[channel][sample] *= gain;
    }

    freeRunningPpq = startPpq + static_cast<double> (numSamples) * phaseIncrement;
    for (int channel = numChannels; channel < getTotalNumInputChannels(); ++channel)
        buffer.clear (channel, 0, numSamples);
}

bool GoatranceGaterAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* GoatranceGaterAudioProcessor::createEditor()
{
    return new GoatranceGaterAudioProcessorEditor (*this);
}

void GoatranceGaterAudioProcessor::getStateInformation (juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destination);
}

void GoatranceGaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GoatranceGaterAudioProcessor();
}
