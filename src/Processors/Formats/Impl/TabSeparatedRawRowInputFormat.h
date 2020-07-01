#pragma once

#include <Core/Block.h>
#include <Formats/FormatSettings.h>
#include <IO/ReadBufferFromString.h>
#include <Processors/Formats/RowInputFormatWithDiagnosticInfo.h>


namespace DB
{

/** A stream to input data in tsv format, but without escaping individual values.
  * It only supports columns without '\n' or '\t'
  */
class TabSeparatedRawRowInputFormat : public TabSeparatedRowInputFormat
{
public:
    /** with_names - the first line is the header with the names of the columns
      * with_types - on the next line header with type names
      */
    TabSeparatedRawRowInputFormat(
        const Block & header_,
        ReadBuffer & in_,
        const Params & params_,
        bool with_names_,
        bool with_types_,
        const FormatSettings & format_settings_)
        : TabSeparatedRowInputFormat(header_, in_, params_, with_names_, with_types_, format_settings_)
    {
    }

    String getName() const override { return "TabSeparatedRawRowInputFormat"; }

    bool readField(IColumn & column, const DataTypePtr & type, bool) override
    {
        char * pos = find_first_symbols<'\n', '\t'>(in.position(), in.buffer().end());
        ReadBufferFromMemory cell(in.position(), pos - in.position());

        type->deserializeAsWholeText(column, cell, format_settings);

        in.position() = pos;

        return true;
    }
};

}
