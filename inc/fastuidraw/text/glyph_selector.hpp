/*!
 * \file glyph_selector.hpp
 * \brief file glyph_selector.hpp
 *
 * Copyright 2016 by Intel.
 *
 * Contact: kevin.rogovin@intel.com
 *
 * This Source Code Form is subject to the
 * terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with
 * this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 *
 * \author Kevin Rogovin <kevin.rogovin@intel.com>
 *
 */


#pragma once

#include <fastuidraw/text/font.hpp>
#include <fastuidraw/text/glyph.hpp>
#include <fastuidraw/text/glyph_cache.hpp>

namespace fastuidraw
{
/*!\addtogroup Text
  @{
*/

  /*!
    \brief
    A GlyphSelector performs the act of selecting a glyph
    from a font preference and a character code.
   */
  class GlyphSelector:public reference_counted<GlyphSelector>::default_base
  {
  public:

    /*!
      \brief
      A FontGeneratorBase is a means to create a font. Adding a font
      via a FontGenerator allows one to avoid opening and creating
      the font until the font is actually needed.
     */
    class FontGeneratorBase:public reference_counted<FontGeneratorBase>::default_base
    {
    public:
      /*!
        To be implemented by a derived class to generate a font.
       */
      virtual
      reference_counted_ptr<const FontBase>
      generate_font(void) const = 0;

      /*!
        To be implemented by a derived class to return the FontProperties
        of the font that would be generated by generate_font().
       */
      virtual
      const FontProperties&
      font_properties(void) const = 0;
    };

    /*!
      \brief
      A FontGroup represents a group of fonts which is selected
      from a FontProperties. The data of a FontGroup is entirely
      opaque. In addition, if the GlyphSelector that created the
      FontGroup goes out of scope, the object behind the opaque
      FontGroup also goes out of scope and thus the FontGroup
      should be discarded and no longer used.
     */
    class FontGroup
    {
    public:
      FontGroup(void):
        m_d(nullptr)
      {}

      /*!
        Returns a list of fonts of this FontGroup that have been
        loaded.
       */
      c_array<const reference_counted_ptr<const FontBase> >
      loaded_fonts(void) const;

      /*!
        Returns a list of generators of this FontGroup that have
        not yet loaded their font.
       */
      c_array<const reference_counted_ptr<const FontGeneratorBase> >
      font_generators(void) const;

    private:
      friend class GlyphSelector;
      void *m_d;
    };

    /*!
      Ctor
      \param cache GlyphCache to store/fetch glyphs.
     */
    explicit
    GlyphSelector(reference_counted_ptr<GlyphCache> cache);

    ~GlyphSelector();

    /*!
      Add a font to this GlyphSelector.
      \param h font to add
     */
    void
    add_font(reference_counted_ptr<const FontBase> h);

    /*!
      Add a font via \ref FontGeneratorBase to this GlyphSelector
      \param gen the object that will generate the font
     */
    void
    add_font_generator(reference_counted_ptr<const FontGeneratorBase> gen);

    /*!
      Fetch a font from a FontProperties description. The return
      value will be the closest matched font added with add_font().
      \param props FontProperties by which to search
      \param exact_match if true, only consider those fonts which have, except
                         forFontProperties::source_label(), the same values for
                         each field of \ref FontProperties in FontBase::properties().
     */
    reference_counted_ptr<const FontBase>
    fetch_font(const FontProperties &props, bool exact_match = false);

    /*!
      Fetch a FontGroup from a FontProperties value
      \param props font properties used to generate group.
      \param exact_match if true, only consider those groups which have, except
                         forFontProperties::source_label(), the same values for
                         each field of \ref FontProperties in FontBase::properties().
     */
    FontGroup
    fetch_group(const FontProperties &props, bool exact_match);

    /*!
      Fetch a Glyph (and if necessary generate it and place into GlyphCache)
      with font merging from a glyph rendering type, font properties and character code.
      \param tp glyph rendering type.
      \param props font properties used to fetch font
      \param character_code character code of glyph to fetch
      \param exact_match if true, only consider those glyphs from those
                         fonts which has, except forFontProperties::source_label(),
                         the same values for each field of \ref FontProperties in
                         FontBase::properties().
     */
    Glyph
    fetch_glyph(GlyphRender tp, const FontProperties &props, uint32_t character_code,
                bool exact_match = false);

    /*!
      Fetch a Glyph (and if necessary generate it and place into GlyphCache)
      with font merging from a glyph rendering type, font properties and character code.
      \param tp glyph rendering type.
      \param group FontGroup used to fetch font
      \param character_code character code of glyph to fetch
      \param exact_match if true, only consider those glyphs from those
                         fonts which has, except forFontProperties::source_label(),
                         the same values for each field of \ref FontProperties in
                         FontBase::properties().
     */
    Glyph
    fetch_glyph(GlyphRender tp, FontGroup group, uint32_t character_code,
                bool exact_match = false);

    /*!
      Fetch a Glyph (and if necessary generate it and place into GlyphCache)
      with font merging from a glyph rendering type, font preference and character code.
      \param tp glyph rendering type.
      \param h handle to font from which to fetch the glyph, if the glyph
               is not present in the font attempt to get the glyph from
               a font of similiar properties
      \param character_code character code of glyph to fetch
      \param exact_match if true, only consider those glyphs from those
                         fonts which has, except forFontProperties::source_label(),
                         the same values for each field of \ref FontProperties in
                         FontBase::properties().
     */
    Glyph
    fetch_glyph(GlyphRender tp,
                reference_counted_ptr<const FontBase> h,
                uint32_t character_code,
                bool exact_match = false);

    /*!
      Fetch a Glyph (and if necessary generate it and place into GlyphCache)
      without font merging from a glyph rendering type, font and character code.
      \param tp glyph rendering type
      \param h handle to font from which to fetch the glyph, if the glyph
               is not present in the font, then return an invalid Glyph.
      \param character_code character code of glyph to fetch
     */
    Glyph
    fetch_glyph_no_merging(GlyphRender tp, reference_counted_ptr<const FontBase> h,
                           uint32_t character_code);

    /*!
      Fill Glyph values from an iterator range of character code values.
      \tparam input_iterator read iterator to type that is castable to uint32_t
      \tparam output_iterator write iterator to Glyph
      \param tp glyph rendering type
      \param group FontGroup to choose what font
      \param character_codes_begin iterator to 1st character code
      \param character_codes_end iterator to one past last character code
      \param output_begin begin iterator to output
      \param exact_match if true, only consider those glyphs from those
                         fonts which has, except forFontProperties::source_label(),
                         the same values for each field of \ref FontProperties in
                         FontBase::properties().
     */
    template<typename input_iterator,
             typename output_iterator>
    void
    create_glyph_sequence(GlyphRender tp, FontGroup group,
                          input_iterator character_codes_begin,
                          input_iterator character_codes_end,
                          output_iterator output_begin,
                          bool exact_match = false);

    /*!
      Fill Glyph values from an iterator range of character code values.
      \tparam input_iterator read iterator to type that is castable to uint32_t
      \tparam output_iterator write iterator to Glyph
      \param tp glyph rendering type
      \param h handle to font from which to fetch the glyph, if the glyph
               is not present in the font attempt to get the glyph from
               a font of similiar properties
      \param character_codes_begin iterator to 1st character code
      \param character_codes_end iterator to one past last character code
      \param output_begin begin iterator to output
      \param exact_match if true, only consider those glyphs from those
                         fonts which has, except forFontProperties::source_label(),
                         the same values for each field of \ref FontProperties in
                         FontBase::properties().
     */
    template<typename input_iterator,
             typename output_iterator>
    void
    create_glyph_sequence(GlyphRender tp,
                          reference_counted_ptr<const FontBase> h,
                          input_iterator character_codes_begin,
                          input_iterator character_codes_end,
                          output_iterator output_begin,
                          bool exact_match = false);

    /*!
      Fill an array of Glyph values from an array of character code values.
      \tparam input_iterator read iterator to type that is castable to uint32_t
      \tparam output_iterator write iterator to Glyph
      \param tp glyph rendering type
      \param h handle to font from which to fetch the glyph, if the glyph
               is not present in the font attempt to get the glyph from
               a font of similiar properties
      \param character_codes_begin iterator to first character code
      \param character_codes_end iterator to one pash last character code
      \param output_begin begin iterator to output
     */
    template<typename input_iterator,
             typename output_iterator>
    void
    create_glyph_sequence_no_merging(GlyphRender tp,
                                     reference_counted_ptr<const FontBase> h,
                                     input_iterator character_codes_begin,
                                     input_iterator character_codes_end,
                                     output_iterator output_begin);

  private:
    void
    lock_mutex(void);

    void
    unlock_mutex(void);

    Glyph
    fetch_glyph_no_lock(GlyphRender tp, FontGroup group, uint32_t character_code,
                        bool exact_match);

    Glyph
    fetch_glyph_no_lock(GlyphRender tp,
                        reference_counted_ptr<const FontBase> h,
                        uint32_t character_code,
                        bool exact_match);

    Glyph
    fetch_glyph_no_merging_no_lock(GlyphRender tp,
                                   reference_counted_ptr<const FontBase> h,
                                   uint32_t character_code);

    void *m_d;
  };

  template<typename input_iterator,
           typename output_iterator>
  void
  GlyphSelector::
  create_glyph_sequence(GlyphRender tp, FontGroup group,
                        input_iterator character_codes_begin,
                        input_iterator character_codes_end,
                        output_iterator output_begin,
                        bool exact_match)
  {
    lock_mutex();
    for(;character_codes_begin != character_codes_end; ++character_codes_begin, ++output_begin)
      {
        uint32_t v;
        v = static_cast<uint32_t>(*character_codes_begin);
        *output_begin = fetch_glyph_no_lock(tp, group, v, exact_match);
      }
    unlock_mutex();
  }

  template<typename input_iterator,
           typename output_iterator>
  void
  GlyphSelector::
  create_glyph_sequence(GlyphRender tp,
                        reference_counted_ptr<const FontBase> h,
                        input_iterator character_codes_begin,
                        input_iterator character_codes_end,
                        output_iterator output_begin,
                        bool exact_match)
  {
    lock_mutex();
    for(;character_codes_begin != character_codes_end; ++character_codes_begin, ++output_begin)
      {
        uint32_t v;
        v = static_cast<uint32_t>(*character_codes_begin);
        *output_begin = fetch_glyph_no_lock(tp, h, v, exact_match);
      }
    unlock_mutex();
  }

  template<typename input_iterator,
           typename output_iterator>
  void
  GlyphSelector::
  create_glyph_sequence_no_merging(GlyphRender tp,
                                   reference_counted_ptr<const FontBase> h,
                                   input_iterator character_codes_begin,
                                   input_iterator character_codes_end,
                                   output_iterator output_begin)
  {
    lock_mutex();
    for(;character_codes_begin != character_codes_end; ++character_codes_begin, ++output_begin)
      {
        uint32_t v;
        v = static_cast<uint32_t>(*character_codes_begin);
        *output_begin = fetch_glyph_no_merging_no_lock(tp, h, v);
      }
    unlock_mutex();
  }

/*! @} */
}
