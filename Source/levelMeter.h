

#pragma once
#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class levelMeter : public juce::Component
	{
	public:

		void paint(juce::Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat();

			g.setColour(juce::Colours::antiquewhite);
			g.fillRoundedRectangle(bounds, 5.f);

			g.setColour(juce::Colours::sandybrown.withBrightness(0.4f));
			const auto scaledX = juce::jmap(level, -60.f, 6.f, 0.f, static_cast<float>(getHeight()));
			g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 5.f);
		}

		void setLevel(const float value) { level = value; }
	private:
		float level = -60.f;
	};
}