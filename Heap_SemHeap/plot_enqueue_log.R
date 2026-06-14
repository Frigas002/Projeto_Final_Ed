# Grafico de Insercao - Escala Logaritmica
dados <- read.csv(file.choose())

plot(
  dados$Tamanho,
  log10(dados$Insercao_Sem_Heap + 1),
  type = "b",
  col = "red",
  pch = 19,
  ylim = range(log10(dados[, 2:3] + 1)),
  xlab = "Tamanho da fila",
  ylab = "log10(comparações + 1)",
  main = "Inserção: Sem Heap vs Com Heap (log)"
)

lines(
  dados$Tamanho,
  log10(dados$Insercao_Com_Heap + 1),
  type = "b",
  col = "blue",
  pch = 19
)

legend(
  "topleft",
  legend = c("Inserção sem Heap", "Inserção com Heap"),
  col = c("red", "blue"),
  pch = 19,
  lty = 1
)
