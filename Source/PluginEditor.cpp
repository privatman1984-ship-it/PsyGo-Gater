#include "PluginEditor.h"

namespace
{
const juce::Colour background (0xff17111e);
const juce::Colour panel (0xff281a31);
const juce::Colour accent (0xffd344ff);
const juce::Colour text (0xfff6e8ff);
const juce::Colour muted (0xffbd9ccc);
}

GoatranceGaterAudioProcessorEditor::GoatranceGaterAudioProcessorEditor (GoatranceGaterAudioProcessor& processor)
    : AudioProcessorEditor (&processor), audioProcessor (processor),
      loopAttachment (processor.parameters, "loopBars", loopBox)
{
    setSize (760, 420);
    setResizable (true, true);
    setResizeLimits (680, 380, 1000, 600);

    title.setText ("GOATRANCE  /  GATER", juce::dontSendNotification);
    title.setJustificationType (juce::Justification::centredLeft);
    title.setFont (juce::Font (juce::FontOptions (25.0f, juce::Font::bold)));
    title.setColour (juce::Label::textColourId, text);
    addAndMakeVisible (title);

    subtitle.setText ("V2  •  SWING / ENVELOPE / 16-STEP PATTERN", juce::dontSendNotification);
    subtitle.setJustificationType (juce::Justification::centredLeft);
    subtitle.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    subtitle.setColour (juce::Label::textColourId, muted);
    addAndMakeVisible (subtitle);

    loopBox.addItem ("1 BAR", 1);
    loopBox.addItem ("2 BARS", 2);
    loopBox.addItem ("4 BARS", 3);
    loopBox.setColour (juce::ComboBox::backgroundColourId, panel);
    loopBox.setColour (juce::ComboBox::textColourId, text);
    loopBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff63456f));
    addAndMakeVisible (loopBox);

    loopLabel.setText ("LOOP", juce::dontSendNotification);
    loopLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    loopLabel.setColour (juce::Label::textColourId, muted);
    addAndMakeVisible (loopLabel);

    const juce::StringArray names { "depth", "wet", "attackMs", "releaseMs", "swing" };
    const juce::StringArray captions { "DEPTH", "WET", "ATTACK ms", "RELEASE ms", "SWING" };
    for (int i = 0; i < knobs.size(); ++i)
    {
        auto& slider = knobs[static_cast<size_t> (i)];
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 19);
        slider.setNumDecimalPlacesToDisplay (i < 2 || i == 4 ? 0 : 1);
        slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
        slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff493452));
        slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffffca65));
        slider.setColour (juce::Slider::textBoxTextColourId, text);
        slider.setColour (juce::Slider::textBoxBackgroundColourId, panel);
        addAndMakeVisible (slider);

        knobLabels[static_cast<size_t> (i)].setText (captions[i], juce::dontSendNotification);
        knobLabels[static_cast<size_t> (i)].setJustificationType (juce::Justification::centred);
        knobLabels[static_cast<size_t> (i)].setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
        knobLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, muted);
        addAndMakeVisible (knobLabels[static_cast<size_t> (i)]);
        sliderAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.parameters, names[i], slider));
    }

    for (int step = 0; step < 16; ++step)
    {
        auto button = std::make_unique<juce::TextButton> (juce::String (step + 1));
        button->setClickingTogglesState (true);
        button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff382541));
        button->setColour (juce::TextButton::buttonOnColourId, accent);
        button->setColour (juce::TextButton::textColourOffId, juce::Colour (0xffd9c5e1));
        button->setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        stepAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
            audioProcessor.parameters, "step" + juce::String (step + 1), *button));
        addAndMakeVisible (*button);
        stepButtons.push_back (std::move (button));
    }
}

void GoatranceGaterAudioProcessorEditor::paint (juce::Graphics& graphics)
{
    graphics.fillAll (background);
    graphics.setColour (panel);
    graphics.fillRoundedRectangle (getLocalBounds().toFloat().reduced (14.0f), 14.0f);
    graphics.setColour (accent);
    graphics.fillRoundedRectangle (juce::Rectangle<float> (24.0f, 24.0f, 5.0f, 44.0f), 2.0f);
    graphics.setColour (juce::Colour (0xff52395d));
    graphics.drawRoundedRectangle (juce::Rectangle<float> (24.0f, 250.0f, getWidth() - 48.0f, 92.0f), 9.0f, 1.0f);
}

void GoatranceGaterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (30);
    title.setBounds (bounds.removeFromTop (32));
    subtitle.setBounds (bounds.removeFromTop (20));
    bounds.removeFromTop (12);

    auto top = bounds.removeFromTop (180);
    auto loopArea = top.removeFromLeft (108);
    loopLabel.setBounds (loopArea.removeFromTop (20));
    loopBox.setBounds (loopArea.removeFromTop (30));
    top.removeFromLeft (10);

    const auto width = top.getWidth() / static_cast<int> (knobs.size());
    for (int i = 0; i < knobs.size(); ++i)
    {
        auto area = top.removeFromLeft (width);
        knobLabels[static_cast<size_t> (i)].setBounds (area.removeFromTop (20));
        knobs[static_cast<size_t> (i)].setBounds (area.removeFromTop (150));
    }

    bounds.removeFromTop (12);
    auto grid = bounds.removeFromTop (75);
    constexpr int gap = 7;
    const auto cellWidth = (grid.getWidth() - 15 * gap) / 16;
    for (int step = 0; step < 16; ++step)
        stepButtons[static_cast<size_t> (step)]->setBounds (
            grid.getX() + step * (cellWidth + gap), grid.getY() + 10, cellWidth, 48);
}
