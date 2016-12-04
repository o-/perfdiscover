library(ggplot2)
library(reshape2)
X11()
res <- read.csv("res.csv")
res2 <- melt(res, id.vars=c("WS","EL"))
ggplot(res2, aes(x=WS, y=value, color=EL)) +
  stat_summary(fun.data = "mean_cl_boot") +
  facet_wrap(~EL) +
  scale_y_continuous(trans="log2")
Sys.sleep(1000)
