#pragma once

#include "PluginProcessor.h"
#include <memory>
#include <vector>

class GoatranceGaterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GoatranceGaterLookAndFeel()
    {
        setColour (juce::ComboBox::arrowColourId, juce::Colour (0xffffca65));
    }
};

class GoatranceGaterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GoatranceGaterAudioProcessorEditor (GoatranceGaterAudioProcessor&);
    ~GoatranceGaterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void configureSlider (juce::Slider&, const juce::String&);

    GoatranceGaterAudioProcessor& audioProcessor;
    GoatranceGaterLookAndFeel lookAndFeel;
    juce::Label title, subtitle, loopLabel;
    juce::ComboBox loopBox;
    juce::Slider depthSlider, wetSlider;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment loopAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment depthAttachment, wetAttachment;
    std::vector<std::unique_ptr<juce::Label>> sliderLabels;
    std::vector<std::unique_ptr<juce::TextButton>> stepButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> stepAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoatranceGaterAudioProcessorEditor)
};
