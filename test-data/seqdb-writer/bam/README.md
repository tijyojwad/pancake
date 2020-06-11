The filtered dataset was created like so:
    dataset filter subreadset.xml filtered.subreadset.xml 'zm!=2' 'length>=40000'

Unfiltered reads and their lengths:
- subreads1.bam:
    ref1/1/0_42148 42148
    ref1/2/0_42124 42124
    ref1/3/0_42176 42176
    ref1/4/0_42179 42179
    ref1/5/0_42280 42280
- subreads2.bam:
    ref2/1/0_42148 42148
    ref2/2/0_42124 42124
    ref2/3/0_42176 42176
    ref2/4/0_42179 42179
    ref2/5/0_42280 42280
- subreads3.bam:
    ref1/158/0_23696 23696
    eef1/159/0_42173 42173
    eef1/160/0_42090 42090
    eef1/161/0_38694 38694
    ref1/162/0_42180 42180

Expected reads with the filter applied:
    ref1/1/0_42148 42148
    ref1/3/0_42176 42176
    ref1/4/0_42179 42179
    ref1/5/0_42280 42280
    ref2/1/0_42148 42148
    ref2/3/0_42176 42176
    ref2/4/0_42179 42179
    ref2/5/0_42280 42280
    ref1/159/0_42173 42173
    ref1/160/0_42090 42090
    ref1/162/0_42180 42180
