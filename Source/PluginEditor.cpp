#include "PluginEditor.h"

GoatranceGaterAudioProcessorEditor::GoatranceGaterAudioProcessorEditor (GoatranceGaterAudioProcessor& processor)
    : AudioProcessorEditor (&processor), audioProcessor (processor),
      loopAttachment (processor.parameters, "loopBars", loopBox),
      depthAttachment (processor.parameters, "depth", depthSlider),
      wetAttachment (processor.parameters, "wet", wetSlider)
{
    setSize (620, 360);
    setResizable (true, true);
    setResizeLimits (520, 320, 900, 520);

    title.setText ("GOATRANCE  /  GATER", juce::dontSendNotification);
    title.setJustificationType (juce::Justification::centredLeft);
    title.setFont (juce::Font (juce::FontOptions (25.0f, juce::Font::bold)));
    title.setColour (juce::Label::textColourId, juce::Colour (0xfff6e8ff));
    addAndMakeVisible (title);

    subtitle.setText ("16-STEP RHYTHMIC GATE", juce::dontSendNotification);
    subtitle.setJustificationType (juce::Justification::centredLeft);
    subtitle.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    subtitle.setColour (juce::Label::textColourId, juce::Colour (0xffbd9ccc));
    addAndMakeVisible (subtitle);

    configureSlider (depthSlider, "DEPTH");
    configureSlider (wetSlider, "WET");

    loopBox.addItem ("1 BAR", 1);
    loopBox.addItem ("2 BARS", 2);
    loopBox.addItem ("4 BARS", 3);
    loopBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff271d32));
    loopBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xfff6e8ff));
    loopBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff63456f));
    addAndMakeVisible (loopBox);

    loopLabel.setText ("LOOP", juce::dontSendNotification);
    loopLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    loopLabel.setColour (juce::Label::textColourId, juce::Colour (0xffbd9ccc));
    addAndMakeVisible (loopLabel);

    for (int step = 0; step < 16; ++step)
    {
        auto button = std::make_unique<juce::TextButton> (juce::String (step + 1));
        button->setClickingTogglesState (true);
        button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff382541));
        button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffd344ff));
        button->setColour (juce::TextButton::textColourOffId, juce::Colour (0xffd9c5e1));
        button->setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        stepAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
            audioProcessor.parameters, "step" + juce::String (step + 1), *button));
        addAndMakeVisible (*button);
        stepButtons.push_back (std::move (button));
    }

    setLookAndFeel (&lookAndFeel);
}

GoatranceGaterAudioProcessorEditor::~GoatranceGaterAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void GoatranceGaterAudioProcessorEditor::configureSlider (juce::Slider& slider, const juce::String& labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 20);
    slider.setTextValueSuffix (" %");
    slider.setNumDecimalPlacesToDisplay (0);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffd344ff));
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff493452));
    slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffffca65));
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff6e8ff));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff271d32));
    addAndMakeVisible (slider);

    auto label = std::make_unique<juce::Label>();
    label->setText (labelText, juce::dontSendNotification);
    label->setJustificationType (juce::Justification::centred);
    label->setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    label->setColour (juce::Label::textColourId, juce::Colour (0xffbd9ccc));
    addAndMakeVisible (*label);
    sliderLabels.push_back (std::move (label));
}

void GoatranceGaterAudioProcessorEditor::paint (juce::Graphics& graphics)
{
    graphics.fillAll (juce::Colour (0xff17111e));
    graphics.setColour (juce::Colour (0xff281a31));
    graphics.fillRoundedRectangle (getLocalBounds().toFloat().reduced (14.0f), 14.0f);
    graphics.setColour (juce::Colour (0xffd344ff));
    graphics.fillRoundedRectangle (juce::Rectangle<float> (24.0f, 25.0f, 5.0f, 43.0f), 2.0f);
    graphics.setColour (juce::Colour (0xff52395d));
    graphics.drawRoundedRectangle (juce::Rectangle<float> (24.0f, 113.0f, getWidth() - 48.0f, 102.0f), 9.0f, 1.0f);
}

void GoatranceGaterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (28);
    title.setBounds (bounds.removeFromTop (32));
    subtitle.setBounds (bounds.removeFromTop (21));
    bounds.removeFromTop (13);

    auto controls = bounds.removeFromTop (91);
    loopLabel.setBounds (controls.removeFromLeft (68).withY (controls.getY() + 8).withHeight (18));
    loopBox.setBounds (loopLabel.getX(), loopLabel.getBottom() + 5, 96, 30);
    controls.removeFromLeft (24);

    const auto knobWidth = 100;
    for (int i = 0; i < 2; ++i)
    {
        auto area = controls.removeFromLeft (knobWidth);
        sliderLabels[static_cast<size_t> (i)]->setBounds (area.withHeight (18));
        (i == 0 ? depthSlider : wetSlider).setBounds (area.withY (area.getY() + 12).withHeight (76));
    }

    bounds.removeFromTop (20);
    auto grid = bounds.removeFromTop (82);
    const auto gap = 7;
    const auto cellWidth = (grid.getWidth() - 15 * gap) / 16;
    for (int step = 0; step < 16; ++step)
        stepButtons[static_cast<size_t> (step)]->setBounds (grid.getX() + step * (cellWidth + gap), grid.getY() + 13, cellWidth, 48);
}
