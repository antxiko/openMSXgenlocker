#ifndef MSXNMS8280VIDEO_HH
#define MSXNMS8280VIDEO_HH

#include "MSXDevice.hh"

namespace openmsx {

/** Video/audio control ports of the Philips NMS 8280 (I/O ports 0xF6..0xF8).
  *
  * Based on the NMS 8280 service manual (block diagram) and on the MSX2 sub-ROM
  * of that machine:
  *  - 0xF6 ("mode select 1"): bit 5 = 0 selects the 50% video mix (the
  *    DIZ/YMIX/B-W outputs go to the analog unit).
  *  - 0xF7 ("mode select 2"): bits 7:6 are the inverted S1:S0 bits of VDP R#9
  *    (they drive the mode indicators), the other bits select the
  *    external video/audio routing. The sub-ROM reads this port back at boot to
  *    detect the hardware.
  *  - 0xF8: audio mix control (written by the sub-ROM, shadow in 0xFFF8).
  * Only the 50% video mix has an effect on the emulation for now; the other
  * bits are just stored and can be read back.
  */
class MSXNMS8280Video final : public MSXDevice
{
public:
	explicit MSXNMS8280Video(const DeviceConfig& config);

	void reset(EmuTime time) override;
	[[nodiscard]] byte readIO(uint16_t port, EmuTime time) override;
	[[nodiscard]] byte peekIO(uint16_t port, EmuTime time) const override;
	void writeIO(uint16_t port, byte value, EmuTime time) override;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	void applyVideoMix() const;

private:
	byte portF6 = 0xFF;
	byte portF7 = 0xFF;
	byte portF8 = 0x00;
};

} // namespace openmsx

#endif
