#include "SpeakerSetup.hpp"

#include "aiira_parser.hpp"
#include "converter.hpp"
#include "csv_parser.hpp"
#include "ease_parser.hpp"
#include "fourdsound_parser.hpp"
#include "spat_parser.hpp"
#include "spat_revolution_parser.hpp"
#include "speakerview_parser.hpp"

namespace spat
{
static std::string detectSpeakerFileFormat(const QString& filePath)
{
  QFileInfo info(filePath);
  QString ext = info.suffix().toLower();

  if(ext == "ease")
    return "EASE";
  if(ext == "json")
    return "IEM";
  if(ext == "rtf")
    return "Spat (IRCAM)";
  if(ext == "csv")
    return "CSV";
  if(ext == "ioconfig")
    return "Spat Revolution";
  if(ext == "xml")
  {
    // Need to check content to distinguish between SpatGRIS and 4D Sound
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream stream(&file);
      QString content = stream.read(100);
      if(content.contains("<setup"))
        return "4D Sound";
      else if(content.contains("<SPEAKER_SETUP"))
        return "SpatGRIS";
    }
    return "SpatGRIS";
  }

  return "";
}

static auto parseSpeakerFile(std::string_view format, std::string_view content)
{
  using namespace spatparse;
  std::optional<spatparse::unified::loudspeaker_configuration> unified_config;

  if(format == "EASE")
  {
    if(auto parsed = spatparse::ease::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format == "IEM")
  {
    if(auto parsed = spatparse::aiira::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format == "Spat (IRCAM)")
  {
    if(auto parsed = spatparse::spat::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format == "CSV")
  {
    if(auto parsed = spatparse::csv::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format.starts_with("SpatGRIS"))
  {
    if(auto parsed = spatparse::spatgris::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format == "4D Sound")
  {
    if(auto parsed = spatparse::fourdsound::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }
  else if(format == "Spat Revolution")
  {
    if(auto parsed = spatparse::spat_revolution::parse(content))
    {
      unified_config.emplace();
      convert(*parsed, *unified_config);
    }
  }

  return unified_config;
}

std::function<void(SpeakerSetup&)> SpeakerSetup::in_t::obj_t::process(file_type data)
{
  auto type = detectSpeakerFileFormat(
      QString::fromUtf8(data.filename.data(), data.filename.size()));
  if(type == "")
    return {};

  QFile f(data.filename.data());
  if(f.open(QIODevice::ReadOnly))
  {
    if(auto res = parseSpeakerFile(type, f.readAll()))
    {
      return [r = std::move(*res)](SpeakerSetup& self) {
        self.m_last_speakers.assign(r.loudspeakers.begin(), r.loudspeakers.end());
        self.update_outputs();
      };
    }
  }
  return {};
}

void SpeakerSetup::update_outputs()
{
  switch(inputs.output_mode)
  {
    case OutputMode::Direct: {
      auto& out_v = outputs.speakers.value.emplace<0>();
      out_v.clear();
      for(auto& sp : m_last_speakers)
        out_v.push_back(sp);
      break;
    }
    case OutputMode::OnlyPositions:
      auto& out_v = outputs.speakers.value.emplace<1>();
      out_v.clear();
      for(auto& sp : m_last_speakers)
        out_v.push_back({float(sp.x), float(sp.y), float(sp.z)});
      break;
  }
}
}
