#!/usr/bin/env R

library(ggplot2)
library(reshape2)

w <- function() readLines("stdin", 1)

multmerge = function(mypath){
  filenames=list.files(path=mypath, full.names=TRUE)
  datalist = lapply(filenames, function(x){read.csv(file=x,header=T)})
  Reduce(function(x,y) {rbind(x,y)}, datalist)
}

args <- commandArgs(trailingOnly = TRUE)

res <- multmerge(args[1])
res2 <- melt(res, id.vars=c("WS", "ES", "ORD"))

X11()

pl <- ggplot(res2, aes(x=WS, y=value, color=ORD)) +
  stat_summary(fun.data = "mean_cl_boot", size=0.2) +
  facet_wrap(ORD~ES, ncol=4, scale="free") +
  geom_vline(xintercept=c(15,18,as.numeric(args[3])), linetype = "longdash", color="gray")
  scale_y_continuous(trans="log10")
# w()
ggsave(filename=paste0("plot_",args[2],".pdf"), plot=pl, width=20, height=20, units="cm")
