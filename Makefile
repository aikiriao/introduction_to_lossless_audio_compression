RM := rm
LATEX := uplatex
DVIPDFMX := dvipdfmx

# LATEXFLAGS := -interaction=batchmode

TARGET := introduction_to_lossless_audio_compression.pdf
TARGET_BASENAME := $(basename $(TARGET))

.PHONY: all clean distclean

all: $(TARGET)

clean:
	$(RM) -f *.aux *.log *.dvi *.bcf *.blg *.run.xml *.bbl *.toc *.out *.snm *.nav *.idx

distclean:
	make clean
	$(RM) -f $(TARGET)

rebuild:
	make distclean
	make all

%.pdf: %.dvi
	$(DVIPDFMX) $<

%.dvi: %.tex
	$(LATEX) $(LATEXFLAGS) $<
	$(LATEX) $(LATEXFLAGS) $< # 参照を確実に通すため
