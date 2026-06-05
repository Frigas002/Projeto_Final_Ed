#utiliza log10() p eixo Y para ser possivel de analisar os valores pequenos
dados <- read.csv(file.choose())

plot(
  dados$Tamanho,
  log10(dados$Insercao_Sem_Heap + 1),
  type = "b",
  col = "red",
  pch = 19,
  ylim = range(log10(dados[, 2:5] + 1)),
  xlab = "Tamanho da fila",
  ylab = "log10(comparações + 1)",
  main = "Comparações em escala logarítmica"
)

lines(dados$Tamanho, log10(dados$Insercao_Com_Heap + 1), type = "b", col = "blue", pch = 19)
lines(dados$Tamanho, log10(dados$Remocao_Sem_Heap + 1), type = "b", col = "darkgreen", pch = 19)
lines(dados$Tamanho, log10(dados$Remocao_Com_Heap + 1), type = "b", col = "purple", pch = 19)

legend(
  "topleft",
  legend = c(
    "Inserção sem Heap",
    "Inserção com Heap",
    "Remoção sem Heap",
    "Remoção com Heap"
  ),
  col = c("red", "blue", "darkgreen", "purple"),
  pch = 19,
  lty = 1
)