library(ggplot2)
library(scales)
library(rdsg)

load_level <- function(file_path) {
  dat <- read.csv(file_path, sep="\t", header=FALSE)
  colnames(dat) <- c("parent", "level", "id")
  dat$parent <- NULL
  return(dat)
}

read_data <- function(file_path) {
  dat <- read.csv(file_path, sep="\t", header=FALSE)
  colnames(dat) <- c("certainty", "nparent", "parent",
                     "id", "pcc_intersect", "pc_intersect",
                     "cpc_intersect", "cparent", "cparentc", "cchild")
  
  dat <- subset(dat, nparent != 1)
  dat <- subset(dat, (certainty - (1/nparent)) / certainty >= 0)
  dat <- transform(dat, certainty = ifelse((certainty - (1/nparent)) / certainty > 0,
                                           (certainty - (1/nparent)) / certainty, 0))
  
  dat$score = (dat$pc_intersect) / (dat$cchild + dat$cparent - dat$pc_intersect)  
  return(dat)
}

dat534366 <- read_data("../../coverage/534366_variable_cate_hier_by_certainty.tsv")
dat534366_005 <- read_data("../../coverage/534366_0.05_variable_cate_hier_by_certainty.tsv")
dat534366_095 <- read_data("../../coverage/534366_0.95_variable_cate_hier_by_certainty.tsv")
dat26700_095 <- read_data("../../coverage/26700_0.95_variable_cate_hier_by_certainty.tsv")
dat26700_005 <- read_data("../../coverage/26700_0.05_variable_cate_hier_by_certainty.tsv")
dat26700 <- read_data("../../coverage/26700_variable_cate_hier_by_certainty.tsv")

dat26700_005$root <- rep("Science (\u03D2=0.05)", nrow(dat26700_005))
dat26700$root <- rep("Science (\u03D2=0.25)", nrow(dat26700))
dat26700_095$root <- rep("Science (\u03D2=0.95)", nrow(dat26700_095))
dat534366$root <- rep("Obama (\u03D2=0.25)", nrow(dat534366))
dat534366_005$root <- rep("Obama (\u03D2=0.05)", nrow(dat534366_005))
dat534366_095$root <- rep("Obama (\u03D2=0.95)", nrow(dat534366_095))

#dat <- rbind(dat534366, dat534366_005, dat534366_095, dat26700_005, dat26700_095, dat26700)
dat <- rbind(dat534366_005, dat534366_095, dat26700_005, dat26700_095)


dat$certainty = cut_interval(dat$certainty, n=5)

dat.summarySE <- summarySE(data = dat, measurevar = "score", groupvars = c("certainty","root", "draw"))
dat <- dat.summarySE

# ggplot(dat, aes(x=certainty, y=score, fill=root)) +
#   geom_bar(stat="identity",
#            colour="black", # border
#            alpha = 1, position="dodge") + # fill aplha
#   scale_fill_grey(start = .5, end = .9) + 
#   geom_errorbar(aes(ymin=score - se, ymax = score + se), position=position_dodge(0.9), width=.1) +
#   theme_bw()

#dat_slice <- subset(dat, root != "Obama (\u03D2=0.25)")
#p1 <- ggplot(dat_slice, aes(x=certainty, y=score, color=colors, linetype=colors, shape=colors, group=root)) +

my.labs <- list(bquote(paste("Obama (", gamma, "=0.05)")),bquote(paste("Obama (", gamma, "=0.95)")))
pobama <- ggplot(subset(dat, grepl("Obama", root)), aes(x=certainty, y=score, color=root, shape=root, linetype=root, group=root)) +
  geom_point()+
  geom_line()+
  scale_y_continuous(expand=c(0,0), limits=c(min(dat$score - dat$se), max(dat$score+dat$se)))+  
  scale_x_discrete(expand=c(0,0))+
  ylab("Jaccard Coefficient") +
  xlab("Certainty") +
  geom_errorbar(aes(ymin=score - se, ymax = score + se), width=.05) +
  scale_color_manual(values = dsg_color(num = 2, prefer="matlab"), breaks=c("Obama (\u03D2=0.05)","Obama (\u03D2=0.95)"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("Obama (\u03D2=0.05)","Obama (\u03D2=0.95)"), labels=my.labs) +
  scale_shape_manual(values = c(16,17), breaks=c("Obama (\u03D2=0.05)","Obama (\u03D2=0.95)"), labels=my.labs) +
  dsg_theme()
ggsave(pobama, filename = "jaccard_obama.eps", width = 5, height = 5)

my.labs <- list(bquote(paste("Science (", gamma, "=0.05)")),bquote(paste("Science (", gamma, "=0.95)")))
pscience <- ggplot(subset(dat, grepl("Science", root)), aes(x=certainty, y=score, color=root, shape=root, linetype=root, group=root)) +
  geom_point() +
  geom_line() +
  scale_y_continuous(expand=c(0,0), limits=c(min(dat$score - dat$se), max(dat$score+dat$se)))+
  scale_x_discrete(expand=c(0,0))+
  ylab("Jaccard Coefficient") +
  xlab("Certainty") +
  geom_errorbar(aes(ymin=score - se, ymax = score + se), width=.05) +
  scale_color_manual(values = dsg_color(num = 2, prefer="matlab"), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs) +
  scale_shape_manual(values = c(16,17), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs) +
  dsg_theme()
ggsave(pscience, filename = "jaccard_science.eps", width = 5, height = 5)

ddd <- rbind(dat534366_005, dat534366_095, dat26700_005, dat26700_095)
#ddd$certainty = cut_interval(ddd$certainty, n=4)


dist_dat <- dat
sum_dat <- summaryBy(N ~ root, data=dat, FUN=(sum))
dist_dat$N <- dat$N / sum_dat[,2]
ggplot(subset(dist_dat, grepl("Obama", root)), aes(x=certainty,y=N, group=root, linetype=root, color=root)) + 
  scale_color_manual(values = c("#d7191c", "#fdae61")) +
  geom_line(size=1.4) + theme_bw() + ylab("") +
  theme(axis.line = element_line(color = "black"),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank(),
        panel.border = element_blank(),
        panel.background = element_blank(),
        axis.text = element_text(size=16),
        axis.title.y = element_text(size=18),
        axis.title.x = element_text(size=18),
        legend.position = "bottom",
        legend.text = element_text(size=16),
        legend.title = element_blank())

ggplot(subset(ddd, grepl("Science", root)), aes(x=certainty, fill=root, color=root, linetype=root)) +
  geom_density(alpha=0.3) +
  theme_bw() +
  theme(axis.line = element_line(color = "black"),
        panel.grid.major = element_blank(),
        panel.grid.minor = element_blank(),
        panel.border = element_blank(),
        panel.background = element_blank(),
        axis.text = element_text(size=16),
        axis.title.y = element_text(size=18),
        axis.title.x = element_text(size=18),
        legend.position = "bottom",
        legend.text = element_text(size=16),
        legend.title = element_blank())


cairo_ps(file="certainty_density_obama_rb.eps", width=5, height=5)
my.labs <- list(bquote(paste("Obama (", gamma, "=0.05)")),bquote(paste("Obama (", gamma, "=0.95)")))
ggplot(subset(ddd, grepl("Obama", root)), aes(x=certainty, color=root, linetype=root)) + geom_density(alpha=0.5) +
  scale_color_manual(values = dsg_color(num=2,prefer = "matlab"), breaks=c("Obama (\u03D2=0.05)","Obama (\u03D2=0.95)"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("Obama (\u03D2=0.05)","Obama (\u03D2=0.95)"), labels=my.labs)+
  scale_y_continuous(expand = c(0,0), limits=c(0,2.1))+
  scale_x_continuous(expand = c(0,0), limits=c(0,1), breaks=c(0.25,0.5,0.75,1))+
  ylab("Density") + xlab("Certainty")+
  dsg_theme()
dev.off()

cairo_ps(file="certainty_density_science_rb.eps", width=5, height=5)
my.labs <- list(bquote(paste("Science (", gamma, "=0.05)")),bquote(paste("Science (", gamma, "=0.95)")))
ggplot(subset(ddd, grepl("Science", root)), aes(x=certainty, color=root, linetype=root)) +
  geom_density(alpha=0.05) +
  scale_color_manual(values = dsg_color(num=2,prefer = "matlab"), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs) +
  scale_fill_manual(values = dsg_color(num=2,prefer = "matlab"), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("Science (\u03D2=0.05)","Science (\u03D2=0.95)"), labels=my.labs)+
  scale_y_continuous(expand = c(0,0), limits=c(0,2.1))+
  scale_x_continuous(expand = c(0,0), limits=c(0,1), breaks=c(0.25,0.5,0.75,1))+
  ylab("Density") + xlab("Certainty")+
  dsg_theme()
dev.off()

ggsave("./certainty_density_obama.eps", width = 5, height = 5)

# p2 <- ggplot(subset(dat, root %in% c("Obama (\u03D2=0.25)", "Science (\u03D2=0.25)")), aes(x=certainty, y=score, color=colors, linetype=root, shape=root, group=root)) +
#   geom_point(position=position_dodge(0.05))+
#   geom_line(position=position_dodge(0.05))+
#   ylim(min(dat$score), max(dat$score))+
#   ylab("Dice Coefficient") +
#   xlab("Certainty") +
#   geom_errorbar(aes(ymin=score - se, ymax = score + se), width=.2, position=position_dodge(0.05)) +
#   theme_bw() +
#   theme(axis.line = element_line(color = "black"),
#         panel.grid.major = element_blank(),
#         panel.grid.minor = element_blank(),
#         panel.border = element_blank(),
#         panel.background = element_blank(),
#         #axis.text = element_text(size=16),
#         #axis.title.y = element_text(size=18),
#         #axis.title.x = element_text(size=18),
#         legend.position = "bottom",
#         #legend.text = element_text(size=16),
#         legend.title = element_blank())

#multiplot(p1, p2, cols=2)
