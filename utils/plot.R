#!/usr/bin/env R

library(ggplot2)
library(reshape2)

w <- function() readLines("stdin", 1)

multmerge = function(mypath){
  filenames=list.files(path=mypath, full.names=TRUE)
  datalist = lapply(filenames, function(x){read.csv(file=x,header=T)})
  Reduce(function(x,y) {rbind(x,y)}, datalist)
}

res <- multmerge("out")
res2 <- melt(res, id.vars=c("WS", "ES", "ORD"))

X11()

ggplot(res2, aes(x=WS, y=value, color=ORD)) +
  stat_summary(fun.data = "mean_cl_boot") +
  facet_wrap(~ES, ncol=3) +
  scale_y_continuous(trans="log2")
w()