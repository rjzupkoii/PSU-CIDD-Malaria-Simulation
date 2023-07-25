# Malaria Simulation Manual

This directory contains the files necessary to build the Malaria Simulation manual, which has been prepared using [R Markdown](https://rmarkdown.rstudio.com/) and [bookdown](https://bookdown.org/). The contents of the directory are organized as follows:

**Files**\
`index.Rmd` - Configuration of the manual\
`_bookdown.yml` - Order of chapters in the manual\
`_output.yml` - Configuration of `bookdown`

**Directories / Folders**\
`chapters` - Contains the chapters of the manual, one chapter per file.\
`appendices` - Contains the appendices of the manual, one appendix per file.\
`tex` - Custom TeX files used to configure the layout of the manual.

Additionally, the `demo` and `examples` directories contain the code for the simulation demo and example configuration YAML files respectively.

## Building the Manual
The easiest way to edit and build the manual is via [RStudio](https://posit.co/products/open-source/rstudio/) which includes support for source and visual editing along with partial builds of chapters via the **Knit** menu option. The full manual can be built via the **Build** tab, then the **Build Book** command. The newly compiled PDF will overwrite the `manual.pdf` file in the root of this directory.