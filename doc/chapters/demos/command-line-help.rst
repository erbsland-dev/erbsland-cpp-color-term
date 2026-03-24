..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************
Command Line Help
*****************

``command-line-help`` demonstrates how a regular command-line tool can use colorful output and
``Terminal::printParagraph()`` to keep a help screen readable across different terminal widths.

The demo prints directly into the normal shell history. On wider terminals it uses paragraph layout for wrapped
descriptions, indents, and wrap markers. If the effective width drops below 40 columns, it deliberately falls back
to simple line-based output so you can compare both approaches.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/command-line-help
    $ ./cmake-build-debug/demo-apps/command-line-help --terminal-width=72

ANSI Output Example
===================

The following excerpt shows the rich help output with a deterministic simulated width:

.. erbsland-ansi::
    :escape-char: ␛

    ␛[?25l␛[92mcommand-line-help␛[37m [␛[35m<options>␛[37m]
    
    ␛[97mSummary
    ␛[37mThis demo prints a fictive command-line help page that adapts to the␛[97m       ␛[39m
    ␛[37mterminal width. On wider terminals it uses ␛[97mTerminal::printParagraph()␛[37m to␛[97m   ␛[39m
    ␛[37mkeep descriptions aligned, wrapped, and easy to scan. Below 40 columns it␛[97m  ␛[39m
    ␛[37mintentionally falls back to plain line-oriented output so you can compare␛[97m  ␛[39m
    ␛[37mboth styles.␛[97m                                                               ␛[39m
    
    ␛[97mPreview Paragraph
    ␛[93;44mPreview ␛[97mThis paragraph keeps a visible background so options like␛[49m          ␛[39m
    ␛[97;44mbackground mode, wrap markers, word breaks, ellipsis handling, and␛[49m         ␛[39m
    ␛[97;44malignment become easier to inspect while you adjust the rendering settings.␛[39;49m
    
    ␛[97mOptions
    ␛[96m--help␛[37m/␛[93m-h␛[97m                          ␛[37mRender this formatted help output. The␛[97m  ␛[39m
    ␛[97m                                   ␛[37mflag is mostly here so the demo behaves␛[97m ␛[39m
    ␛[97m                                   ␛[37mlike a familiar command-line tool.␛[97m      ␛[39m
    ␛[96m--terminal-width␛[37m/␛[93m-t␛[37m=␛[35m<columns>␛[39m      ␛[37mDisable automatic width detection and␛[39m   
                                       ␛[37msimulate a terminal width between 20 and␛[39m
                                       ␛[37m200 cells for deterministic wrapping.␛[39m   
    ␛[96m--description-column␛[37m/␛[93m-c␛[37m=␛[35m<column>␛[39m   ␛[37mOverride the description tab stop for␛[39m   
                                       ␛[37mthe options list with a fixed value.␛[39m    
                                       ␛[37mValid values are 12 to 60.␛[39m              
    ␛[96m--maximum-description-column␛[37m/␛[93m-d␛[37m=␛[35m<column>␛[39m                                   
                                       ␛[37mCap the automatically chosen description␛[39m
                                       ␛[37mtab stop. The default is 35 to force␛[39m    
                                       ␛[37msome wrapping in the options list. Valid␛[39m
                                       ␛[37mvalues are 20 to 60.␛[39m                    
    ␛[96m--alignment␛[37m/␛[93m-a␛[37m=␛[35m<left|center|right>␛[39m ␛[37mChoose the horizontal alignment used for␛[39m
                                       ␛[37mthe wrapped preview paragraphs.␛[39m         
    ␛[96m--line-indent␛[37m/␛[93m-l␛[37m=␛[35m<columns>␛[39m         ␛[37mIndent the preview paragraphs by 0 to 10␛[39m
                                       ␛[37mcolumns before any wrapping takes place.␛[39m
    ␛[96m--first-line-indent␛[37m/␛[93m-f␛[37m=␛[35m<columns>␛[39m   ␛[37mOverride the first-line indent for␛[39m      
                                       ␛[37mpreview paragraphs. Valid values are 0␛[39m  
                                       ␛[37mto 12.␛[39m                                  
    ␛[96m--wrapped-line-indent␛[37m/␛[93m-w␛[37m=␛[35m<columns>␛[39m ␛[37mOverride the indentation of wrapped␛[39m     
                                       ␛[37mlines. Values between 0 and 30 make the␛[39m 
                                       ␛[37meffect easy to inspect.␛[39m                 
    ␛[96m--background-mode␛[37m/␛[93m-b␛[37m=␛[35m<mode>␛[39m        ␛[37mSet the background fill strategy:␛[39m       
                                       ␛[37mdefault, wrapped-left, wrapped-right,␛[39m   
                                       ␛[37mwrapped-both, full-right, or full-both.␛[39m 
    ␛[96m--line-break-start-mark␛[37m/␛[93m-s␛[37m=␛[35m<text>␛[39m  ␛[37mInsert a one- or two-character marker at␛[39m
                                       ␛[37mthe start of each wrapped continuation␛[39m  
                                       ␛[37mline.␛[39m                                   
    ␛[96m--line-break-end-mark␛[37m/␛[93m-m␛[37m=␛[35m<text>␛[39m    ␛[37mAppend a one- or two-character marker at␛[39m
                                       ␛[37mthe right edge when a line wraps.␛[39m       
    ␛[96m--paragraph-spacing␛[37m/␛[93m-p␛[37m=␛[35m<single|double>␛[39m                                     
                                       ␛[37mSwitch between compact paragraphs or␛[39m    
                                       ␛[37mdouble-spaced output with one empty row␛[39m 
                                       ␛[37min between.␛[39m                             
    ␛[96m--word-separators␛[37m/␛[93m-i␛[37m=␛[35m<tokens>␛[39m      ␛[37mUse comma-separated separator tokens␛[39m    
                                       ␛[37msuch as space,tab,slash or one-character␛[39m
                                       ␛[37mliterals like ; and |.␛[39m                  
    ␛[96m--word-break-mark␛[37m/␛[93m-k␛[37m=␛[35m<char>␛[39m        ␛[37mSet the single character inserted when a␛[39m
                                       ␛[37mword is split because it does not fit on␛[39m
                                       ␛[37mthe current line.␛[39m                       
    ␛[96m--maximum-line-wraps␛[37m/␛[93m-r␛[37m=␛[35m<count>␛[39m    ␛[37mLimit the number of automatic wraps per␛[39m 
                                       ␛[37mparagraph. Use 0 for unlimited or values␛[39m
                                       ␛[37mup to 8 to trigger ellipsis behaviour.␛[39m  
    ␛[96m--paragraph-ellipsis-mark␛[37m/␛[93m-x␛[37m=␛[35m<text>␛[39m                                        
                                       ␛[37mChoose the marker that signals clipped␛[39m  
                                       ␛[37mparagraphs after the configured wrap␛[39m    
                                       ␛[37mlimit has been reached.␛[39m                 
    ␛[96m--tab-stops␛[37m/␛[93m-u␛[37m=␛[35m<list>␛[39m              ␛[37mProvide comma-separated tab stops like␛[39m  
                                       ␛[37m1,24,40 or use wrapped to align a stop␛[39m  
                                       ␛[37mwith the wrapped-line indent.␛[39m           
    ␛[96m--on-error␛[37m/␛[93m-o␛[37m=␛[35m<plain|empty>␛[39m        ␛[37mChoose the fallback when the paragraph␛[39m  
                                       ␛[37mcannot be laid out: plain output or␛[39m     
                                       ␛[37mempty output.␛[39m                           
    
    ␛[97mTry It
    ␛[37mExperiment with ␛[96m--terminal-width␛[37m, wrap markers, custom tab stops, and the␛[97m  ␛[39m
    ␛[37mpreview paragraph settings to see how a real command help screen can stay␛[97m  ␛[39m
    ␛[37mreadable across narrow and wide terminals.␛[97m                                 ␛[39m
    
    ␛[?25h␛[0m␛[?25h
    ␛[0m␛[?25h

Features Shown
==============

This demo highlights several practical techniques for text-heavy console tools:

* ``Terminal::printParagraph()`` for structured, width-aware command help.
* Mixed-style ``String`` values for realistic command signatures.
* Tab-stop based alignment of option descriptions.
* Configurable paragraph wrapping, break marks, ellipsis handling, and fallback behaviour.
* A plain-output fallback for very narrow terminals.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/command-line-help/src/CommandLineHelpDemo.cpp`

This file contains the argument parsing, help document construction, and the two output paths for wide and narrow
terminal widths.
