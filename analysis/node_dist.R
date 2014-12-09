library(ggplot2)


add_name <- function(df, name) {
  df$label <- name
  return(df)
}

load_dt <- function(path, g, iter, name) {
  df <- read.csv(path, header=FALSE, sep="\t")
  colnames(df) <- c("parent", "level", "child")
  df$gamma <- as.factor(g)
  df$iter <- as.factor(iter)
  return(add_name(df, name))
}

# Load data

sci_005_100 <- load_dt("../../dist/26700_100_0.05_page_graph.txt", 0.05, 100, "Science \u03D2=0.05 iter=100")
sci_005_200 <- load_dt("../../dist/26700_200_0.05_page_graph.txt", 0.05, 200,  "Science \u03D2=0.05 iter=200")
sci_005_300 <- load_dt("../../dist/26700_300_0.05_page_graph.txt", 0.05, 300,  "Science \u03D2=0.05 iter=300")
sci_005_390 <- load_dt("../../dist/26700_390_0.05_page_graph.txt", 0.05, 400,  "Science \u03D2=0.05 iter=390")

sci_005 <- rbind(sci_005_100, sci_005_200, sci_005_300, sci_005_390)

sci_095_100 <- load_dt("../../dist/26700_100_0.95_page_graph.txt", 0.95, 100,"Science \u03D2=0.95 iter=100")
sci_095_200 <- load_dt("../../dist/26700_200_0.95_page_graph.txt", 0.95, 200,"Science \u03D2=0.95 iter=200")
sci_095_300 <- load_dt("../../dist/26700_300_0.95_page_graph.txt", 0.95, 300,"Science \u03D2=0.95 iter=300")
sci_095_390 <- load_dt("../../dist/26700_390_0.95_page_graph.txt", 0.95, 400,"Science \u03D2=0.95 iter=390")

sci_095 <- rbind(sci_095_100, sci_095_200, sci_095_300, sci_095_390)

sci <- rbind(sci_005, sci_095)

oba_005_100 <- load_dt("../../dist/534366_100_0.05_page_graph.txt", 0.05, 100,  "Obama \u03D2=0.05 iter=100")
oba_005_200 <- load_dt("../../dist/534366_200_0.05_page_graph.txt", 0.05, 200,  "Obama \u03D2=0.05 iter=200")
oba_005_300 <- load_dt("../../dist/534366_300_0.05_page_graph.txt", 0.05, 300,  "Obama \u03D2=0.05 iter=300")
oba_005_390 <- load_dt("../../dist/534366_390_0.05_page_graph.txt", 0.05, 400,  "Obama \u03D2=0.05 iter=390")

oba_005 <- rbind(oba_005_100, oba_005_200, oba_005_300, oba_005_390)

oba_095_100 <- load_dt("../../dist/534366_100_0.95_page_graph.txt", 0.95, 100,  "Obama \u03D2=0.95 iter=100")
oba_095_200 <- load_dt("../../dist/534366_200_0.95_page_graph.txt", 0.95, 200,  "Obama \u03D2=0.95 iter=200")
oba_095_300 <- load_dt("../../dist/534366_300_0.95_page_graph.txt", 0.95, 300,  "Obama \u03D2=0.95 iter=300")
oba_095_390 <- load_dt("../../dist/534366_390_0.95_page_graph.txt", 0.95, 400,  "Obama \u03D2=0.95 iter=390")

oba_095 <- rbind(oba_095_100, oba_095_200, oba_095_300, oba_095_390)

oba <- rbind(oba_005, oba_095)

oba_final <- rbind(oba_005_390, oba_095_390)
oba_final$label <- substr(oba_final$label, 1,12)
sci_final <- rbind(sci_005_390, sci_095_390)
sci_final$label <- substr(sci_final$label, 1,14)

both <- rbind(oba_final, sci_final) 

# Empirical Cumulative Density of Document Nodes

my.labs <- list(bquote(paste("Obama (", gamma, "=0.05)")),bquote(paste("Obama (", gamma, "=0.95)")))

poba_final <- ggplot(oba_final, aes(x=level, color=gamma, linetype=gamma)) +
  scale_color_manual(values = dsg_color(num = 2, prefer="matlab"), breaks=c("0.05","0.95"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("0.05","0.95"), labels=my.labs)+
  scale_y_continuous(expand = c(0,0))+
  scale_x_continuous(limits=c(1, max(both$level)), expand = c(0,0))+
  stat_ecdf(geom="line")+
  ylab("") + xlab("Depth")+
  dsg_theme()

ggsave("./ecdf_obama.eps", width = 5, height = 5)
  

my.labs <- list(bquote(paste("Science (", gamma, "=0.05)")),bquote(paste("Science (", gamma, "=0.95)")))


psci_final <- ggplot(sci_final, aes(x=level, color=gamma, linetype=gamma)) +
  scale_color_manual(values = dsg_color(num = 2, prefer="matlab"), breaks=c("0.05","0.95"), labels=my.labs) +
  scale_linetype_manual(values = c(1,2), breaks=c("0.05","0.95"), labels=my.labs)+
  scale_y_continuous(expand = c(0,0))+
  scale_x_continuous(limits=c(1, max(both$level)), expand = c(0,0))+
  stat_ecdf(geom="line")+
  ylab("") + xlab("Depth")+
  dsg_theme()

ggsave("./ecdf_science.eps", width = 5, height = 5, encoding="Greek")



ggplot(both, aes(x=level, color=label, linetype=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level") + theme_bw() + 
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

ggplot(sci_005, aes(x=level, color=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level")

ggplot(sci_095, aes(x=level, color=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level")

ggplot(sci, aes(x=level, color=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level") +
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


ggplot(oba_005, aes(x=level, color=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level")

ggplot(oba_095, aes(x=level, color=label)) + stat_ecdf(geom="line") + theme_classic() +
  ylab("") + xlab("Category Level")

ggplot(oba, aes(x=level, color=label)) + stat_ecdf(geom="line") +
  ylab("") + xlab("Category Level") + scale_x_discrete(limits=1:max(oba$level))

# Topic size

sci_size <- summaryBy(parent ~ level + parent + label, data=sci, FUN=c(length))
sci_size.summary <- summarySE(data=sci_size, measurevar = "parent.length", groupvars = c("label"))
sci_size.summary$label <- substr(sci_size.summary$label, 1,14)
sci_size.summary$iter <- rep(c(100,200,300,390),2)

oba_size <- summaryBy(parent ~ level + parent + label, data=oba, FUN=c(length))
oba_size.summary <- summarySE(data=oba_size, measurevar = "parent.length", groupvars = c("label"))
oba_size.summary$label <- substr(oba_size.summary$label, 1,12)
oba_size.summary$iter <- rep(c(100,200,300,390),2)

ggplot(rbind(sci_size.summary, oba_size.summary), 
       aes(x=iter, y=parent.length, group=label, color=label, linetype=label)) +
  geom_line(size=1.4) + theme_bw() + 
  ylab("Topic Size") +
  xlab("Gibbs Sampling Iteration") +
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