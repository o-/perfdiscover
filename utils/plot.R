#!/usr/bin/env R

library(ggplot2)
library(reshape2)

multmerge = function(mypath){
  filenames=list.files(path=mypath, full.names=TRUE)
  datalist = lapply(filenames, function(x){read.csv(file=x,header=T)})
  Reduce(function(x,y) {rbind(x,y)}, datalist)
}

res <- multmerge("out")
res2 <- melt(res, id.vars=c("WS", "ES", "ORD"))

X11()

ggplot(res2, aes(x=WS, y=value)) +
  stat_summary(fun.data = "mean_cl_boot") +
  facet_wrap(ES~ORD) +
  scale_y_continuous(trans="log2")
Sys.sleep(1000)
