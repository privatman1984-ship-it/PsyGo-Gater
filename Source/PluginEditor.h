#pragma once

#include "PluginProcessor.h"
#include <array>
#include <memory>
#include <vector>

class GoatranceGaterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GoatranceGaterAudioProcessorEditor (GoatranceGaterAudioProcessor&);
    ~GoatranceGaterAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GoatranceGaterAudioProcessor& audioProcessor;
    juce::Label title, subtitle, loopLabel;
    juce::ComboBox loopBox;
    std::array<juce::Slider, 5> knobs;
    std::array<juce::Label, 5> knobLabels;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment loopAttachment;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::TextButton>> stepButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> stepAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoatranceGaterAudioProcessorEditor)
};
