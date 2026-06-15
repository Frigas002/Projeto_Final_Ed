# Grafico de Remocao - Escala Linear
dados <- read.csv(file.choose())

plot(
  dados$Tamanho,
  dados$Remocao_Sem_Heap,
  type = "b",
  col = "darkgreen",
  pch = 19,
  ylim = range(dados[, 2:3]),
  xlab = "Tamanho da fila",
  ylab = "Número de comparações",
  main = "Remoção: Sem Heap vs Com Heap"
)

lines(
  dados$Tamanho,
  dados$Remocao_Com_Heap,
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
