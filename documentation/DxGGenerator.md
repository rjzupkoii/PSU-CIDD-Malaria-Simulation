# DxG Generator

The DxG Generator is used to calculate the efficacy of therapies in the simulation based upon the therapy's drug 
combination and the genotypes that are present in the YAML file for a given region. While this tool uses the same 
simulation codebase, the executable is separate from the primary simulation (i.e., `masim`) and can be invoked as 
follows:

`./DxGGenerator -i [input]`

With the full scope of options supported:

<pre>
-g                  Get the efficacies for a range of genotypes [from to]
-h / --help         Display the help menu
-i / --input        The YAML configuration to read
-t                  Get the efficacies for a range of therapies [from to]

--ec50              EC50 for AS on C580 only
--iov               AS inter-occasion-variability
--iiv               AS inter-individual-variability
</pre>

Note that the DxG Generator is very sensitive to the YAML configuration since the *full* configuration is still loaded 
prior to the efficacy calculations. As such it is recommended that a second, simplified (i.e., non-spatial) 
configuration be prepared for the DxG Generator. The tool is also only comparable with single therapies (i.e., 
`SFTStrategy`), which can be used in the configuration as follows:

```YAML
strategy_db:
  0:
    name: SFTStrategy
    type: SFT
    therapy_id: 0
initial_strategy_id: 0
```