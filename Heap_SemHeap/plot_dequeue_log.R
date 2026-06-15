# Grafico de Remocao - Escala Logaritmica
dados <- read.csv(file.choose())

plot(
  dados$Tamanho,
  log10(dados$Remocao_Sem_Heap + 1),
  type = "b",
  col = "darkgreen",
  pch = 19,
  ylim = range(log10(dados[, 2:3] + 1)),
  xlab = "Tamanho da fila",
  ylab = "log10(comparações + 1)",
  main = "Remoção: Sem Heap vs Com Heap (log)"
)

lines(
  dados$Tamanho,
  log10(dados$Remocao_Com_Heap + 1),
  type = "b",
  col = "purple",
  pch = 19
)

legend(
  "topleft",
  legend = c("Remoção sem Heap", "Remoção com Heap"),
  col = c("darkgreen", "purple"),
  pch = 19,
  lty = 1
)
