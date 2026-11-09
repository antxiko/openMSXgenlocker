#include "MSXNMS8280Video.hh"

#include "ExternalAudio.hh"
#include "MSXMotherBoard.hh"
#include "VDP.hh"
#include "outer.hh"
#include "serialize.hh"

namespace openmsx {

MSXNMS8280Video::MSXNMS8280Video(const DeviceConfig& config)
	: MSXDevice(config)
	, debuggable(getMotherBoard(), getName() + " ports")
{
	reset(EmuTime::dummy());
}

void MSXNMS8280Video::reset(EmuTime time)
{
	portF6 = 0xFF; // video mix off
	portF7 = 0xFF;
	portF8 = 0x00;
	applyVideoMix();
	applyAudioRouting(time);
}

void MSXNMS8280Video::applyVideoMix() const
{
	// The VDP might not exist yet while this device is being constructed.
	if (auto* vdp = dynamic_cast<VDP*>(getMotherBoard().findDevice("VDP"))) {
		vdp->setVideoMix((portF6 & 0x20) == 0);
		vdp->setVideoDim((portF7 & 0x20) != 0);
	}
}

void MSXNMS8280Video::applyAudioRouting(EmuTime time) const
{
	// bit 1 = 0: external audio in the left channel, bit 0 = 0: in the right one
	if (auto* audio = dynamic_cast<MSXExternalAudio*>(
	        getMotherBoard().findDevice("External audio"))) {
		audio->setRouting((portF7 & 0x02) == 0, (portF7 & 0x01) == 0, time);
	}
}

byte MSXNMS8280Video::readIO(uint16_t port, EmuTime time)
{
	return peekIO(port, time);
}

byte MSXNMS8280Video::peekIO(uint16_t port, EmuTime /*time*/) const
{
	// Bits that are not connected read as 1.
	switch (port & 0xFF) {
	case 0xF6: return portF6 | 0x83;
	case 0xF7: return portF7 | 0x07;
	case 0xF8: return portF8 | 0xFC;
	default:   return 0xFF;
	}
}

void MSXNMS8280Video::writeIO(uint16_t port, byte value, EmuTime time)
{
	switch (port & 0xFF) {
	case 0xF6:
		portF6 = value;
		applyVideoMix();
		break;
	case 0xF7:
		portF7 = value;
		applyVideoMix(); // also the dimming of the video
		applyAudioRouting(time);
		break;
	case 0xF8:
		portF8 = value;
		break;
	default:
		break;
	}
}

// Debuggable

MSXNMS8280Video::Debuggable::Debuggable(MSXMotherBoard& motherBoard_, const std::string& name_)
	: SimpleDebuggable(motherBoard_, name_,
	                   "Latches of the video control ports 0xF6, 0xF7 and 0xF8", 3)
{
}

byte MSXNMS8280Video::Debuggable::read(unsigned address)
{
	const auto& device = OUTER(MSXNMS8280Video, debuggable);
	switch (address) {
	case 0: return device.portF6;
	case 1: return device.portF7;
	case 2: return device.portF8;
	default: return 0xFF;
	}
}

void MSXNMS8280Video::Debuggable::write(unsigned /*address*/, byte /*value*/)
{
	// read-only: the ports are written by the MSX software
}

template<typename Archive>
void MSXNMS8280Video::serialize(Archive& ar, unsigned /*version*/)
{
	ar.template serializeBase<MSXDevice>(*this);
	ar.serialize("portF6", portF6,
	             "portF7", portF7,
	             "portF8", portF8);
	if constexpr (Archive::IS_LOADER) {
		applyVideoMix();
		applyAudioRouting(getCurrentTime());
	}
}
INSTANTIATE_SERIALIZE_METHODS(MSXNMS8280Video);
REGISTER_MSXDEVICE(MSXNMS8280Video, "NMS8280Video");

} // namespace openmsx
